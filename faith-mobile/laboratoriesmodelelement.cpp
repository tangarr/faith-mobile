#include "laboratoriesmodelelement.h"

LaboratoriesModelElement::LaboratoriesModelElement()
{
    _checked = false;
}

LaboratoriesModelElement::~LaboratoriesModelElement()
{

}

bool LaboratoriesModelElement::checked() const
{
    return _checked;
}

void LaboratoriesModelElement::setChecked(bool checked)
{
    _checked = checked;
}
