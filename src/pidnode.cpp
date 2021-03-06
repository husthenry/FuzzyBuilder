#include "pidnode.h"
#include "graphwidget.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <qevent.h>
#include <qnamespace.h>
#include <QDebug>

PidNode::PidNode(GraphWidget *graphWidget) : Node(graphWidget),SetPoint(graphWidget),Input(graphWidget),Direction(graphWidget)
{
    SetPoint.setINTEXT("Requested(SetPoint)");
    Input.setINTEXT("Obtained(Input)");
    Direction.setINTEXT("Direction");
    QRect exposedRect(graphWidget->mapToScene(0,0).toPoint(),graphWidget->viewport()->rect().size());
    setPos(exposedRect.width()/2,exposedRect.height() / 2);
    if (!FindNewVertPosition(-1))
        FindNewVertPosition(1);


}

QRectF PidNode::boundingRect() const
{
int width = 20;
int height = 20;
qreal adjust = 2;
return QRectF( -width - adjust, -height - adjust, 2 * width + adjust, 2 * height + adjust);
}

bool PidNode::AllowAttach(Node *node) const
{
    return (SetPoint.AllowAttach(node) || Input.AllowAttach(node) || Direction.AllowAttach(node));
}

void PidNode::addEdge(Edge *edge)
{
    if (SetPoint.AllowAttach(edge->getSource())) SetPoint.addEdge(edge);
    else {
        if (Input.AllowAttach(edge->getSource())) Input.addEdge(edge);
        else if (Direction.AllowAttach(edge->getSource())) Direction.addEdge(edge);
            else Node::addEdge(edge);// shouldn't get here
    }
}

void PidNode::WriteHeader(QTextStream &h) const
{
}

void PidNode::WriteNodeInfo(QTextStream &s) const
{

    Q_UNUSED(s);

    Node::WriteNodeInfo(s);
}





double PidNode::Simulate()
{
    // todo
    foreach (Edge *edge, edgeList)
    {
        if (edge->getSource()!=this)
        {
            setInValue(edge->getSource()->Simulate());
            //double error =

        }
    }
    return getCurrent();

}

void PidNode::setNewPos(const QPointF &value)
{
    Node::setNewPos(value);
    SetPoint.setPos(value.x()-25,value.y()-20);
    Input.setPos(value.x()-25,value.y()+20);
    Direction.setPos(value.x(),value.y()-20);
}
