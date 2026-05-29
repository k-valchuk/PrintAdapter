#ifndef MAXPEAKVALUE_H
#define MAXPEAKVALUE_H

#include <QObject>
#include <QTimer>

class MaxPeakValue: public QObject {
    Q_OBJECT

public:
    MaxPeakValue();
    MaxPeakValue(const MaxPeakValue& other);
    // Установить новое значение (Если оно меньше предыдущего, вернет false и значение не поменяется)
    bool setValue(int value);
    // Вернуть текущее значение
    int getValue();

    // Установить пересчитанное значение
    void setRecalculateVal(int value);

private:
    int currentValue = 0;
    QTimer timer;

private slots:
    // Сбрасывает значение
    void slotResetValue();
};

#endif // MAXPEAKVALUE_H
