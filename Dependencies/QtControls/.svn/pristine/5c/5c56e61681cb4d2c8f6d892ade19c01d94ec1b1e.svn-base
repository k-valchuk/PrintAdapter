//-------------------------------------------------------------------------------------------------
// Сцена таймлайна
//-------------------------------------------------------------------------------------------------
// - Здесь только отрисовка фона активной части проекта и неактивной
//-------------------------------------------------------------------------------------------------
#ifndef TIMELINESCENE_H
#define TIMELINESCENE_H

#include <QGraphicsScene>

class TimelineScene : public QGraphicsScene
{
public:
    explicit TimelineScene(QObject *parent = nullptr);

    void SetActiveWidth(double w) {active_width_ = w;}
    void SetHeightSceneSpace(int space) {height_scene_space_ = space;}
    void SetSceneColors(const QColor &act_col, const QColor &no_avt_col) {active_color_ = act_col; no_active_color_ = no_avt_col;}

    int GetHeightSceneSpace() const {return height_scene_space_;}

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    double active_width_;           // Длительность проекта
    int height_scene_space_;        // Пространство для корректного закрашивания области
    QColor active_color_;           // Цвет активной области
    QColor no_active_color_;        // Цвет неактивной области

};

#endif // TIMELINESCENE_H
