

#include "config.h"
#include "CSSMediaRule.h"

#include "CSSParser.h"
#include "ExceptionCode.h"

namespace WebCore {

CSSMediaRule::CSSMediaRule(CSSStyleSheet* parent, PassRefPtr<MediaList> media, PassRefPtr<CSSRuleList> rules)
    : CSSRule(parent)
    , m_lstMedia(media)
    , m_lstCSSRules(rules)
{
}

CSSMediaRule::~CSSMediaRule()
{
    if (m_lstMedia)
        m_lstMedia->setParent(0);

    int length = m_lstCSSRules->length();
    for (int i = 0; i < length; i++)
        m_lstCSSRules->item(i)->setParent(0);
}

unsigned CSSMediaRule::append(CSSRule* rule)
{
    if (!rule)
        return 0;

    rule->setParent(this);
    return m_lstCSSRules->insertRule(rule, m_lstCSSRules->length());
}

unsigned CSSMediaRule::insertRule(const String& rule, unsigned index, ExceptionCode& ec)
{
    if (index > m_lstCSSRules->length()) {
        // INDEX_SIZE_ERR: Raised if the specified index is not a valid insertion point.
        ec = INDEX_SIZE_ERR;
        return 0;
    }

    CSSParser p(useStrictParsing());
    RefPtr<CSSRule> newRule = p.parseRule(parentStyleSheet(), rule);
    if (!newRule) {
        // SYNTAX_ERR: Raised if the specified rule has a syntax error and is unparsable.
        ec = SYNTAX_ERR;
        return 0;
    }

    if (newRule->isImportRule()) {
        // FIXME: an HIERARCHY_REQUEST_ERR should also be thrown for a @charset or a nested
        // @media rule.  They are currently not getting parsed, resulting in a SYNTAX_ERR
        // to get raised above.

        // HIERARCHY_REQUEST_ERR: Raised if the rule cannot be inserted at the specified
        // index, e.g., if an @import rule is inserted after a standard rule set or other
        // at-rule.
        ec = HIERARCHY_REQUEST_ERR;
        return 0;
    }

    newRule->setParent(this);
    unsigned returnedIndex = m_lstCSSRules->insertRule(newRule.get(), index);

    // stylesheet() can only return 0 for computed style declarations.
    stylesheet()->styleSheetChanged();

    return returnedIndex;
}

void CSSMediaRule::deleteRule(unsigned index, ExceptionCode& ec)
{
    if (index >= m_lstCSSRules->length()) {
        // INDEX_SIZE_ERR: Raised if the specified index does not correspond to a
        // rule in the media rule list.
        ec = INDEX_SIZE_ERR;
        return;
    }

    m_lstCSSRules->deleteRule(index);

    // stylesheet() can only return 0 for computed style declarations.
    stylesheet()->styleSheetChanged();
}

String CSSMediaRule::cssText() const
{
    String result = "@media ";
    if (m_lstMedia) {
        result += m_lstMedia->mediaText();
        result += " ";
    }
    result += "{ \n";

    if (m_lstCSSRules) {
        unsigned len = m_lstCSSRules->length();
        for (unsigned i = 0; i < len; i++) {
            result += "  ";
            result += m_lstCSSRules->item(i)->cssText();
            result += "\n";
        }
    }

    result += "}";
    return result;
}

} // namespace WebCore
