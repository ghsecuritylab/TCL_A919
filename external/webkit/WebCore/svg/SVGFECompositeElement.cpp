

#include "config.h"

#if ENABLE(SVG) && ENABLE(FILTERS)
#include "SVGFECompositeElement.h"

#include "MappedAttribute.h"
#include "SVGNames.h"
#include "SVGResourceFilter.h"

namespace WebCore {

SVGFECompositeElement::SVGFECompositeElement(const QualifiedName& tagName, Document* doc)
    : SVGFilterPrimitiveStandardAttributes(tagName, doc)
    , m__operator(FECOMPOSITE_OPERATOR_OVER)
{
}

SVGFECompositeElement::~SVGFECompositeElement()
{
}

void SVGFECompositeElement::parseMappedAttribute(MappedAttribute *attr)
{
    const String& value = attr->value();
    if (attr->name() == SVGNames::operatorAttr) {
        if (value == "over")
            set_operatorBaseValue(FECOMPOSITE_OPERATOR_OVER);
        else if (value == "in")
            set_operatorBaseValue(FECOMPOSITE_OPERATOR_IN);
        else if (value == "out")
            set_operatorBaseValue(FECOMPOSITE_OPERATOR_OUT);
        else if (value == "atop")
            set_operatorBaseValue(FECOMPOSITE_OPERATOR_ATOP);
        else if (value == "xor")
            set_operatorBaseValue(FECOMPOSITE_OPERATOR_XOR);
        else if (value == "arithmetic")
            set_operatorBaseValue(FECOMPOSITE_OPERATOR_ARITHMETIC);
    } else if (attr->name() == SVGNames::inAttr)
        setIn1BaseValue(value);
    else if (attr->name() == SVGNames::in2Attr)
        setIn2BaseValue(value);
    else if (attr->name() == SVGNames::k1Attr)
        setK1BaseValue(value.toFloat());
    else if (attr->name() == SVGNames::k2Attr)
        setK2BaseValue(value.toFloat());
    else if (attr->name() == SVGNames::k3Attr)
        setK3BaseValue(value.toFloat());
    else if (attr->name() == SVGNames::k4Attr)
        setK4BaseValue(value.toFloat());
    else
        SVGFilterPrimitiveStandardAttributes::parseMappedAttribute(attr);
}

void SVGFECompositeElement::synchronizeProperty(const QualifiedName& attrName)
{
    SVGFilterPrimitiveStandardAttributes::synchronizeProperty(attrName);

    if (attrName == anyQName()) {
        synchronize_operator();
        synchronizeIn1();
        synchronizeIn2();
        synchronizeK1();
        synchronizeK2();
        synchronizeK3();
        synchronizeK4();
        return;
    }

    if (attrName == SVGNames::operatorAttr)
        synchronize_operator();
    else if (attrName == SVGNames::inAttr)
        synchronizeIn1();
    else if (attrName == SVGNames::in2Attr)
        synchronizeIn2();
    else if (attrName == SVGNames::k1Attr)
        synchronizeK1();
    else if (attrName == SVGNames::k2Attr)
        synchronizeK2();
    else if (attrName == SVGNames::k3Attr)
        synchronizeK3();
    else if (attrName == SVGNames::k4Attr)
        synchronizeK4();
}

bool SVGFECompositeElement::build(SVGResourceFilter* filterResource)
{
    FilterEffect* input1 = filterResource->builder()->getEffectById(in1());
    FilterEffect* input2 = filterResource->builder()->getEffectById(in2());
    
    if (!input1 || !input2)
        return false;
    
    RefPtr<FilterEffect> effect = FEComposite::create(input1, input2, static_cast<CompositeOperationType>(_operator()),
                                        k1(), k2(), k3(), k4());
    filterResource->addFilterEffect(this, effect.release());

    return true;
}

}

#endif // ENABLE(SVG)