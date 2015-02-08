
package proguard.classfile.attribute;

import proguard.classfile.*;
import proguard.classfile.attribute.visitor.AttributeVisitor;

public class ConstantValueAttribute extends Attribute
{
    public int u2constantValueIndex;


    /**
     * Creates an uninitialized ConstantValueAttribute.
     */
    public ConstantValueAttribute()
    {
    }


    /**
     * Creates an initialized ConstantValueAttribute.
     */
    public ConstantValueAttribute(int u2attributeNameIndex,
                                  int u2constantValueIndex)
    {
        super(u2attributeNameIndex);

        this.u2constantValueIndex = u2constantValueIndex;
    }

    
    // Implementations for Attribute.

    public void accept(Clazz clazz, Field field, AttributeVisitor attributeVisitor)
    {
        attributeVisitor.visitConstantValueAttribute(clazz, field, this);
    }
}
