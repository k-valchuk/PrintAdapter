#include "MaxPeakValue.h"

//---------------------------------------
MaxPeakValue::MaxPeakValue() :currentValue(0)
{
    timer.setInterval(1000);
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, this, &MaxPeakValue::slotResetValue);
}

//---------------------------------------
MaxPeakValue::MaxPeakValue(const MaxPeakValue& other)
{
    timer.setInterval(1000);
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, this, &MaxPeakValue::slotResetValue);
}

//---------------------------------------
bool MaxPeakValue::setValue(int value)
{
    if(currentValue <= value){
        currentValue = value;
        timer.stop();
        timer.start();
        return true;
    }
    else
        return false;
}

//---------------------------------------
int MaxPeakValue::getValue()
{
    return currentValue;
}

//---------------------------------------
void MaxPeakValue::setRecalculateVal(int value)
{
    currentValue = value;
}

//---------------------------------------
void MaxPeakValue::slotResetValue()
{
    currentValue = 0;
}
