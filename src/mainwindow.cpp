#include <QMessageBox>
#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "viewsourcedialog.h"
#include "groups.h"
#include "nodefactory.h"
#include "fuzzynode.h"
#include "TFuzzy.h"
#include "Util.h"
#include <QtDebug>
FuzzyNode *fnp =0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    wp = this;
    gti = new GroupTableItem();
    gti->AddGroupItem(0,"All");
    ui->setupUi(this);
    Active = 0;
    SelectNode(0);
    Filename = "";
    createActions();
    createToolBars();
}

void MainWindow::FillCodeUI(Node *np)
{
    {
    QString s;
    QTextStream ts(&s);
    np->setSourceBeenWritten(false);
        np->WriteSourcePlainGuts(ts);
    ui->FunctionBody->setPlainText(s);
    s="";

        ui->FunctionParameters->setText(s);
        s = "";

        ui->FunctionReturn->setText(s);

        ui->FunctionHeader->setText(np->getName());
        s = "";
        //np->WriteSourceGutsReturn(ts);
        ui->FunctionBodyReturn->setText(s);

    }
}

void MainWindow::SelectAllAction(bool Select)
{
    QGraphicsScene *scene = this->ui->graphicsView->scene();
    foreach (QGraphicsItem *item, scene->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;
        if (node->getSelected()!=Select)    {
            node->setSelected(Select);
            node->update();
        }
       }
}

void MainWindow::SetupMinSlider()
{    
const Node *cActive = Active; // use a const here so we can't change the Node at all
    if (cActive->UsesMin()) // are we using the min slider (IOMin in node)
    {
        ui->MinLabel->setVisible(true);
        ui->MinLabel->setText("");//Active->MinText());
        ui->Min->setVisible(true);
        ui->Min->setMinimum(cActive->MinOfMin());

        //on_MinScale_valueChanged(Active->IOMin > 16 ? 512 : 16);

        if (cActive->UsesMinScale()) {
            int v = cActive->getIOMin() > 16 ? 512 : 16;
            if (!ui->MinScale->isVisible()) {// not yet showing, set to reasonable values
                Node *temp = Active;
                Active = 0;
                ui->MinScale->setMinimum(1);
                ui->MinScale->setMaximum(v*2);
                ui->MinScale->setVisible(true);
                Active = temp;
            }
            ui->MinScaleLabel->setVisible(true);
            ui->Min->setMaximum(cActive->MaxOfMin());
        }
        else    {
            ui->MinScale->setVisible(false);
            ui->MinScaleLabel->setVisible(false);
            ui->Min->setMaximum(cActive->MaxOfMin()*NODEHIGHVAL);
        }

        ui->MinTitle->setText(cActive->MinText());
        ui->Min->setStatusTip("The minimum expected value");


        ui->MinText->setText(cActive->getIOMinText());
        ui->MinText->setVisible(true);
        //on_Min_valueChanged(Active->IOMin);
    }
    else {
        ui->MinLabel->setVisible(false);
        ui->Min->setVisible(false);
        ui->MinScale->setVisible(false);
        ui->MinScaleLabel->setVisible(false);
        ui->MinText->setVisible(false);
    }
    //   ui->MinScale->update();
    //   ui->Min->update();
}

void MainWindow::SetupMaxSlider()
{
const Node *cActive = Active;
    if (cActive->UsesMax())  {
        ui->MaxLabel->setVisible(true);
        ui->MaxLabel->setText("");//Active->MaxText());
        ui->Max->setVisible(true);
        ui->Max->setMinimum(cActive->MinOfMax());


        if (cActive->UsesMaxScale()) {
            int v = cActive->getIOMax() > 16 ? 512 : 16;
            if (!ui->MaxScale->isVisible()) {
                ui->MaxScale->setMinimum(1);
                ui->MaxScale->setMaximum(v*2);
            }
            ui->MaxScale->setVisible(true);
            ui->MaxScaleLabel->setVisible(true);
            ui->Max->setMaximum(cActive->MaxOfMax());
        }
        else {
            ui->MaxScale->setVisible(false);
            ui->MaxScaleLabel->setVisible(false);
            ui->Max->setMaximum(cActive->MaxOfMax()*NODEHIGHVAL);
        }
        ui->MaxTitle->setVisible(true);
        ui->MaxTitle->setText(cActive->MaxText());
        ui->Max->setStatusTip("The maximum expected value");
        ui->MaxText->setText(cActive->getIOMaxText());
        ui->MaxText->setVisible(true);
        //on_Max_valueChanged(Active->IOMax);
    }
    else    {
        ui->Max->setVisible(false);
        ui->MaxLabel->setVisible(false);
        ui->MaxScale->setVisible(false);
        ui->MaxScaleLabel->setVisible(false);
        ui->MaxText->setVisible(false);
        ui->MaxTitle->setVisible(false);
    }
    //   ui->Max->update();
    //   ui->MaxScale->update();
}

void MainWindow::SetupExtraSlider()
{
    if (Active->UsesExtra()) {
        ui->ExtraLabel->setVisible(true);
        // this may not be true in all cases but until proven otherwise it will stay
        ui->Extra->setMinimum(Active->MinOfExtra());
        ui->Extra->setMaximum(Active->MaxOfExtra());
        ui->ExtraLabel->setText(Active->ExtraText());
        ui->Extra->setVisible(true);
    }
    else {
        ui->ExtraLabel->setVisible(false);
        ui->Extra->setVisible(false);
    }
}

void MainWindow::SetupTable(FuzzyNode *fnp)
{
    ui->ValueTable->setModel(fnp);
    ui->ValueTable->setVisible(true);
    ui->ValueTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->ValueTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->ValueTable->update();
}

void MainWindow::SelectNode(Node *np)
{
    bool visible = np!=0;
    ui->tabWidget->setVisible(visible);
    ui->LogicLabel->setVisible(visible);
    ui->NodeName->setVisible(visible);
    ui->ValueTable->setVisible(visible);
    ui->MinLabel->setVisible(false);
    ui->Min->setVisible(false);
    ui->MaxLabel->setVisible(false);
    ui->Max->setVisible(false);
    ui->MaxText->setVisible(false);
    ui->ExtraLabel->setVisible(false);
    ui->Extra->setVisible(false);
    ui->SetPoint->setVisible(false);
    ui->ValueTable->setVisible(false);
    ui->Lock->setVisible(true);
    ui->Graph->setVisible(false);
    ui->MinScale->setVisible(false);
    ui->MaxScale->setVisible(false);
    ui->MinText->setVisible(false);
    ui->MaxText->setVisible(false);
    ui->MaxScaleLabel->setVisible(false);
    ui->MinScaleLabel->setVisible(false);

    if (np) {
        Active = np;
        Active->setSelected(true);
        QString ss;
        ui->Min->setValue(Active->getIOMin());
        ui->Max->setValue(Active->getIOMax());
        ui->Extra->setValue(Active->getActiveValue());

        SetupMinSlider();

        SetupMaxSlider();
        SetupExtraSlider();

        switch(np->GetLogicType()) {
        case fSETUP:
            break;
        case fIN:
        {
            double v = Active->UnNormalize(Active->getCurrent());
            ui->Extra->setValue(v);
            on_Extra_valueChanged(v);
        }
            break;
        case fOUT:
        {
            double v = Active->Normalize(Active->getCurrent());
            ui->Min->setValue(v);
            on_Min_valueChanged(v);
        }
            break;
        case fAND:
            ui->MaxLabel->setVisible(true);
            ui->Max->setMinimum(0);
            ui->Max->setValue(np->getActiveValue());
            ui->Max->setMaximum(NODEHIGHVAL);
            ui->Max->setVisible(true);
            ui->MaxText->setVisible(visible);
            on_Max_valueChanged(np->getActiveValue());
            ui->Max->setStatusTip("The maximum of all inputs up to this level");
            break;
        case fOR:
            ui->MinLabel->setVisible(true);
            ui->Min->setMinimum(0);
            ui->Min->setMaximum(NODEHIGHVAL);
            ui->Min->setVisible(true);
            ui->Min->setStatusTip("The minimum of all inputs down to this level");
            on_Min_valueChanged(np->getActiveValue());
            break;
        case fNOT:
        case fDEFINE:
            break;
        case fFUZZY:
        {
            FuzzyNode *fnp = (FuzzyNode *)np;
            fnp->setIOMin(fnp->getInValue());
            fnp->setHeaderData(0,Qt::Horizontal,"Cause");
            fnp->setHeaderData(1,Qt::Horizontal,"Effect");
            SetupMinSlider();
            SetupMaxSlider();
            SetupTable(fnp);
            ui->SetPoint->setVisible(true);
            ui->Graph->setVisible(true);
            ui->Min->setValue(fnp->getInValue()*NODEHIGHVAL);
            on_Min_valueChanged(fnp->getInValue()*NODEHIGHVAL);

            UpdateTableAndGraph(fnp);
        }
            break;
        case fPID:
        {
            PidNode *pnp = (PidNode *)np;
            ui->MinLabel->setVisible(true);
            ui->MinLabel->setText("P");
            ui->Min->setVisible(true);
            ui->Min->setValue(pnp->get_p());
            on_Min_valueChanged(pnp->get_p());

            ui->MaxLabel->setVisible(true);
            ui->MaxLabel->setText("I");
            ui->Max->setValue(pnp->get_i());
            ui->Max->setVisible(true);
            ui->MaxText->setVisible(true);
            on_Max_valueChanged(pnp->get_i());

            ui->ExtraLabel->setVisible(true);
            ui->ExtraLabel->setText("P");
            ui->Extra->setValue(pnp->get_d());
            ui->Extra->setVisible(true);
        }
            break;
        case fTIMER:
            break;
        }
        ui->NodeName->setText(np->getName());
        FillCodeUI(np);
    }
    else Active = 0;
}

MainWindow::~MainWindow()
{
    delete gti;
    delete ui;
}

void MainWindow::Load(QString filename)
{
    QFile src;
    on_actionNew_triggered();
    src.setFileName(filename);
    if (src.open(QIODevice::ReadOnly | QIODevice::Text ))    {
        QTextStream hs(&src);
        on_actionNew_triggered();
        ui->graphicsView->ReadSource(hs);
        src.close();
    }
    Filename = filename;
    ui->actionSave->setText("Save " + Filename);
    ui->actionSave->setEnabled(true);
    ui->GroupSelect->clear();
    for(int i=0;i<gti->rowCount();i++) {
        ui->GroupSelect->addItem(gti->GetItem(i)->Name);
    }

}

void MainWindow::on_actionLoad_triggered()
{
    // load as C source
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    QStringList filters;
        filters << "Arduino files (*.ino)"
                << "library files (*.cpp)"
                << "Any files(*)";

    dialog.setNameFilters (filters);
    dialog.setDefaultSuffix("ino");
    if (dialog.exec()!=QDialog::Accepted)
        return;
    QStringList sf = dialog.selectedFiles();
    QString filename = sf.first();
    dialog.close();
    if (filename.isEmpty())
        return;


    Load(filename);

}

void MainWindow::on_actionSave_triggered()
{
    //Menu Save
    if (Filename!="") {
         SaveFile(Filename,false);
    }
}
void MainWindow::WriteGroups(QTextStream &h)
{
    getGroupTableItem()->WriteNodeInfo(h);
}

void MainWindow::WriteSource(QTextStream &h, QTextStream &s)
{        
    ui->graphicsView->WriteSource(h,s);
}

void MainWindow::SaveFile(QString filename,bool arduino)
{

    QFile h;
    QFile s;

    int ext = filename.lastIndexOf(".");
    if (ext>-1)
        filename.chop(filename.length() - ext);

    h.setFileName(filename + ".h");
    if (arduino)
       s.setFileName(filename+".ino");
    else    {
       s.setFileName(filename + ".c");
        if (h.exists()) h.remove();
    }
    if (s.exists()) s.remove();
    if (arduino){
        if (s.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream ss(&s);
            WriteSource(ss,ss);
        }
    } else {
    if (h.open(QIODevice::WriteOnly | QIODevice::Text ))    {
        if (s.open(QIODevice::WriteOnly | QIODevice::Text ))            {
            QTextStream hs(&h),ss(&s);
            WriteSource(hs, ss);
        }
        h.close();
    }
    }
    s.close();
    Filename = filename;
    if (arduino)    {
     ui->actionSave_arduino->setText("Save (Arduino) " +Filename);
     ui->actionSave_arduino->setEnabled(true);
    } else {
    ui->actionSave->setText("Save " + Filename);
    ui->actionSave->setEnabled(true);
    }
}

void MainWindow::on_actionSave_As_C_Source_triggered()
{
    // save as C source
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter ( tr("CPP files files(*.cpp);;All Files(*.*)") );
    QString filename = dialog.getSaveFileName();
    dialog.close();
    if (filename.isEmpty())
        return;
    SaveFile(filename,false);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}




/*
 * void MainWindow::on_AddLogic_clicked()
{
    // Add Logic
    AddLogic *al = new AddLogic(this);
    int r = al->exec();
    if (r==0) {
        delete al;
        return;
    }
    LOGICTYPE lt = al->GetLogicType();
    QString Name = al->getName();


    Node *node1 = NodeFactory::Create(ui->graphicsView,lt);
    node1->setName(Name);



    //int w = scene->width();
    //int h = scene->height();


    QRect exposedRect(ui->graphicsView->mapToScene(0,0).toPoint(), ui->graphicsView->viewport()->rect().size());
    //QPointF p = node1->pos();

    //QPointF p1 = node1->pos();
    switch(lt) {
    case fSETUP:

        break;
    case fIN:

    case fTIMER:
        break;
    case fOUT:
        break;
    case fFUZZY:
    case fOR:
    case fNOT:
    case fAND:
    case fPID:
        break;
    }
    delete al;
    SelectNode(node1);

}
*/

void MainWindow::on_ValueTable_clicked(const QModelIndex &/*index*/)
{
    //
}

void MainWindow::on_Min_valueChanged(int value)
{
    if (!Active) return;
double v = value;
    if (!Active->UsesMinScale())
            v /= NODEHIGHVAL;
    double Max = Active->getIOMax();
    double MinOfMax = Active->MinOfMax();
    double MaxOfMax = Active->MaxOfMax();

    double Extra = Active->getActiveValue();
    double MinOfExtra = Active->MinOfExtra();
    double MaxOfExtra = Active->MaxOfExtra();
    Active->setIOMin(v);
    SetupMinSlider();

    if (Max!=Active->getIOMax() || MinOfMax!=Active->MinOfMax() || MaxOfMax!=Active->MaxOfMax())
    {
        double v = Active->getIOMax();
        if (!Active->UsesMaxScale())
                v *= NODEHIGHVAL;
        ui->Max->setValue(v);
    }
    if (Extra!=Active->getActiveValue() || MinOfExtra!=Active->MinOfExtra() || MaxOfExtra!=Active->MaxOfExtra())
        SetupExtraSlider();
    Active->update();
}

void MainWindow::on_Max_valueChanged(int value)
{
    if (!Active) return;
    double v = value;
    if (!Active->UsesMaxScale())
            v /= NODEHIGHVAL;

    double Min = Active->getIOMin();
    double MinOfMin = Active->MinOfMin();
    double MaxOfMin = Active->MaxOfMin();

    double Extra = Active->getActiveValue();
    double MinOfExtra = Active->MinOfExtra();
    double MaxOfExtra = Active->MaxOfExtra();

    Active->setIOMax(v);
    SetupMaxSlider();
    if (Active->getIOMin()!=Min || MinOfMin!=Active->MinOfMin() || MaxOfMin!=Active->MaxOfMin())
        SetupMinSlider();
    if (Extra!=Active->getActiveValue() || MinOfExtra!=Active->MinOfExtra() || MaxOfExtra!=Active->MaxOfExtra())
        SetupExtraSlider();
    Active->update();
}
void MainWindow::on_Extra_valueChanged(int value)
{
    if (!Active) return;
    double Max = Active->getIOMax();
    double Min = Active->getIOMin();
    Active->setActiveValue(value);
    Active->setInValue(value);
    SetupExtraSlider();
        if (Max!=Active->getIOMax())
            SetupMaxSlider();
    if (Min!=Active->getIOMin())
        SetupMinSlider();
    Active->update();
    Simulate();
}


void MainWindow::UpdateTableAndGraph(FuzzyNode *fActive)
{
    ui->ValueTable->setModel(0);
    ui->ValueTable->setModel(fActive);
    ui->ValueTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    ui->ValueTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->Graph->clearGraphs();
    ui->Graph->addGraph();
    {
        QVector<double> c(int(NODEHIGHVAL)),e(int(NODEHIGHVAL));
        for(int i=0;i<NODEHIGHVAL;i++) {
            c[i] = i/NODEHIGHVAL;
            e[i] = fActive->fuzzy.Value(c[i]);
        }
        ui->Graph->graph(0)->setData(c,e);
        ui->Graph->xAxis->setLabel("Cause");
        ui->Graph->yAxis->setLabel("Effect");
        ui->Graph->xAxis->setRange(0,1);
        ui->Graph->yAxis->setRange(0,1);
        ui->Graph->replot();

    }

}

void MainWindow::on_SetPoint_clicked()
{
    if (!Active) return;
    FuzzyNode *fActive = (FuzzyNode *) Active;
    double cause = ui->Min->value()  / NODEHIGHVAL;
    double effect = ui->Max->value() / NODEHIGHVAL;

    fActive->fuzzy.TFuzzyAddPoint(cause,effect);

    UpdateTableAndGraph(fActive);
    fActive->update();
}


QString MainWindow::SuggestName(LOGICTYPE t) const
{
    if (this==0) return "oops";
    return ui->graphicsView->SuggestName(t);
}

void MainWindow::on_Lock_clicked()
{
    this->Frozen = (ui->Lock->checkState() == Qt::Checked) || (ui->Lock->checkState() == Qt::PartiallyChecked);
}



void MainWindow::on_ValueTable_entered(const QModelIndex &/*index*/)
{
    FuzzyNode *fActive = (FuzzyNode *)Active;

   // connect(fActive,SIGNAL(editCompleted(const QString &)),this,SLOT(setWindowTitle(QString)));
}

void MainWindow::Simulate()
{
    this->ui->graphicsView->simulate();
}


void MainWindow::on_MinScale_valueChanged(int value)
{
     Q_UNUSED(value);
    if (Active)
    {
        if (Active->UsesMinScale())
            Active->setMinScale(value);
            SetupMinSlider();
    }
}

void MainWindow::on_MaxScale_valueChanged(int value)
{    
    if (!Active) return;
    Active->setMaxScale(value);

    SetupMaxSlider();
}

void MainWindow::on_actionView_as_C_Source_triggered()
{
ViewSourceDialog *vs = new ViewSourceDialog(this);
QString h,s;
QTextStream hs(&h),ss(&s);
    WriteSource(hs,ss);
   vs->SetSource(h,s);
   vs->exec();
   delete vs;
}

#include <QDockWidget>
void MainWindow::on_DeleteBlock_clicked()
{
    QGraphicsScene *scene = this->ui->graphicsView->scene();
    foreach (QGraphicsItem *item, scene->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;
        if (node->getSelected())
        {
            node->DeleteAllEdges();
            scene->items().removeOne(item);
            delete node;
        }
    }
//Load("test");

}
bool MainWindow::getFrozen() const
{
    return Frozen;
}

void MainWindow::setFrozen(bool value)
{
    Frozen = value;
}


void MainWindow::on_actionNew_triggered()
{
    QGraphicsScene *scene = this->ui->graphicsView->scene();
    foreach (QGraphicsItem *item, scene->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;
        node->DeleteAllEdges();
    }
    foreach (QGraphicsItem *item, scene->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;
         scene->items().removeOne(item);
         delete node;
    }
    SelectNode(0);
}

void MainWindow::on_Group_clicked()
{
    Groups *g = new Groups(this);
    QGraphicsScene *scene = this->ui->graphicsView->scene();
    foreach (QGraphicsItem *item, scene->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;
        {
            g->addNode(node->getName());

        }
    }

    g->exec();
    delete g;
    ui->GroupSelect->clear();
    for(int i=0;i<gti->rowCount();i++) {
        ui->GroupSelect->addItem(gti->GetItem(i)->Name);
    }
}
GroupTableItem *MainWindow::getGroupTableItem() const
{
    return gti;
}

void MainWindow::setGti(GroupTableItem *value)
{
    gti = value;
}


Node *MainWindow::findNode(const QString &Name)
{
    QGraphicsScene *scene = this->ui->graphicsView->scene();
    foreach (QGraphicsItem *item, scene->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;
        if (node->getName()==Name)
               return node;
    }
    return 0;
}


void MainWindow::on_LoadTest_clicked()
{

    Load("test");
}

void MainWindow::on_GroupSelect_currentIndexChanged(const QString &arg1)
{
    QGraphicsScene *scene = this->ui->graphicsView->scene();
   for(int i=0;i<gti->rowCount();i++)   {
       if (gti->GetItem(i)->Name==arg1) {
           int groupid = gti->GetItem(i)->GroupID;
           foreach (QGraphicsItem *item, scene->items()) {
               Node *node = qgraphicsitem_cast<Node *>(item);
               if (!node)
                   continue;
               node->setVisible(node->GroupMember(groupid));

           }
           return;
       }
   }
}

void MainWindow::on_DelLogic_clicked()
{
   if (Active!=0) {
       Active->DeleteAllEdges();
       delete Active;
       Active = 0;
   }
}


void MainWindow::on_actionView_as_Arduino_Source_triggered()
{ // view as arduino source
    ViewSourceDialog *vs = new ViewSourceDialog(this);
    QString hs;
    QTextStream ss(&hs);
        WriteSource(ss,ss);
       vs->SetSource(hs);
       vs->exec();

}

void MainWindow::on_actionSave_as_Arduiono_Source_triggered()
{ // save as arduino source
    QFileDialog dialog(this);
    QStringList filters;
        filters << "Arduino files (*.ino)"
                << "library files (*.cpp)";
    dialog.setNameFilters(filters);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    bool repeat = true;
    QString filename ;
    while(repeat) {
        repeat =false;

        if (dialog.exec()!=QDialog::Accepted)
            return;
        QStringList sf = dialog.selectedFiles();
        filename = sf.first();
        QFileInfo file(filename);
        if ((file.suffix()!="ino") && (file.suffix()!="cpp"))
        {
            QMessageBox::information(NULL,"Error","You must provide an extension");
            repeat = true;
        }

    }
    dialog.close();
    if (filename.isEmpty())
        return;
    SaveFile(filename,true);
}

void MainWindow::on_FunctionHeader_editingFinished()
{
    if (!Active) return;
    Active->setName( ui->FunctionHeader->text());
    Active->update();
}

void MainWindow::on_regenerate_clicked()
{
    if (Active) {
        QString os;
        QTextStream ss(&os);
        Active->setSourceBeenWritten(false);
        Active->WriteSourcePlainGuts(ss);
        ui->FunctionBody->setPlainText(os);
        QString Return,Parameters,FunctionReturn;
        bool HasBrackets;
        Active->FunctionData(Return,Parameters,FunctionReturn,HasBrackets);
        Active->setUserGuts("");
        ui->FunctionReturn->setText(Return);
        ui->FunctionParameters->setText(Parameters);
        ui->FunctionBodyReturn->setText(FunctionReturn);        
        ui->Bracket1->setText(HasBrackets ? "{" : "");
        ui->Bracket2->setText(HasBrackets ? "}" : "");
    }
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if (Active==0) return;// No node selected
    if (index==1) {// looking at code
        QString os;
        QTextStream ss(&os);
            Active->WriteSourcePlainGuts(ss);

        QString s;
        s = Active->getUserGuts();
        if (s=="") s = os;
        ui->FunctionBody->setPlainText(s);
        QString Return,Parameters,FunctionReturn;
        bool HasBrackets;
        ui->FunctionHeader->setText(Active->getName());
        Active->FunctionData(Return,Parameters,FunctionReturn,HasBrackets);
        ui->FunctionReturn->setText(Return);
        ui->FunctionParameters->setText(Parameters);
        ui->FunctionBodyReturn->setText(FunctionReturn);
        ui->Bracket1->setText(HasBrackets ? "{" : "");
        ui->Bracket2->setText(HasBrackets ? "}" : "");

        return;
    }
    if (index==0) {
       QString os;
       QTextStream ss(&os);
       Active->WriteSourcePlainGuts(ss);
       QString s;
       s = ui->FunctionBody->toPlainText();
       ui->NodeName->setText(Active->getName());
       //s = Active->getUserGuts();
       QString regen = Active->Regenerate();
       if (regen==os) // no change from regenerated
           Active->setUserGuts("");
        if (s!=os)
            Active->setUserGuts(s);
    }
}

void MainWindow::on_MinText_editingFinished()
{
    if (!Active) return;
    try {
        bool OK;
    double value = ui->MinText->text().toDouble(&OK);
        if (OK)
        {
            if (!Active->UsesMinScale())
                value *= NODEHIGHVAL;
            ui->Min->setValue(value);
        }
        else {
            QMessageBox::information(NULL, "Bad Value","Number Expected");
            ui->MinText->setText(Active->getIOMinText());
        }
    }
    catch(...)
    {

    }
}

void MainWindow::on_MaxText_editingFinished()
{
    if (!Active) return;
    try {
        bool OK;
    double value = ui->MaxText->text().toDouble(&OK);
        if (OK) {
            if (!Active->UsesMaxScale())
                value *= NODEHIGHVAL;
            ui->Max->setValue(value);
        }
        else {
            QMessageBox::information(NULL, "Bad Value","Integer Value Expected");
            ui->MaxText->setText(Active->getIOMaxText());
        }
    }
    catch(...)
    {

    }
}

void MainWindow::on_NodeName_editingFinished()
{
    if (Active==0) return;
    Active->setName(ui->NodeName->text());
    Active->update();
}

void MainWindow::AddIn()
{
    AddNode(fIN);
}

void MainWindow::AddOut()
{
    AddNode(fOUT);
}

void MainWindow::AddOr()
{
    AddNode(fOR);
}

void MainWindow::AddNot()
{
    AddNode(fNOT);
}

void MainWindow::AddAnd()
{
    AddNode(fAND);
}
void MainWindow::AddFuzzy()
{
    AddNode(fFUZZY);
}
void MainWindow::AddSetup()
{
    AddNode(fSETUP);
}

void MainWindow::AddPID()
{
    AddNode(fPID);
}

void MainWindow::AddTimer()
{
    AddNode(fTIMER);
}

void MainWindow::AddDefine()
{
    AddNode(fDEFINE);
}

void MainWindow::AddNode(LOGICTYPE lt)
{


    Node *node = NodeFactory::Create(ui->graphicsView,lt);
    node->setName(SuggestName(lt));
    QRect exposedRect(ui->graphicsView->mapToScene(0,0).toPoint(), ui->graphicsView->viewport()->rect().size());
    SelectAllAction(false);// unselect all nodes
    SelectNode(node);



}


void MainWindow::createToolBars()
{
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(inNodeAct);
    editToolBar->addAction(outNodeAct);
    editToolBar->addAction(orNodeAct);
    editToolBar->addAction(andNodeAct);
    editToolBar->addAction(notNodeAct);
    editToolBar->addAction(fuzzyNodeAct);
    editToolBar->addAction(setupNodeAct);
    editToolBar->addAction(pidNodeAct);
    editToolBar->addAction(timerNodeAct);
    editToolBar->addAction(defineNodeAct);

}

void MainWindow::createActions()
{
    Node *node;
    node = NodeFactory::Create(ui->graphicsView,fIN);
    inNodeAct = new QAction(node->generateIcon(), tr("&In"), this);
    //inNodeAct->setShortcuts(QKeySequence::Undo);
    inNodeAct->setStatusTip(tr("Add an Input Node"));
    connect(inNodeAct, SIGNAL(triggered()), this, SLOT(AddIn()));
    delete node;


    node = NodeFactory::Create(ui->graphicsView,fOUT);
    outNodeAct = new QAction(node->generateIcon(), tr("&Out"), this);
    outNodeAct->setStatusTip(tr("Add an Output Node"));
    connect(outNodeAct, SIGNAL(triggered()), this, SLOT(AddOut()));
    delete node;

    node = NodeFactory::Create(ui->graphicsView,fOR);
    orNodeAct = new QAction(node->generateIcon(), tr("&Or"), this);
    //orNodeAct->setShortcuts(QKeySequence::Undo);
    orNodeAct->setStatusTip(tr("Add an Or Node"));
    connect(orNodeAct, SIGNAL(triggered()), this, SLOT(AddOr()));
    delete node;

    node = NodeFactory::Create(ui->graphicsView,fAND);
    andNodeAct = new QAction(node->generateIcon(), tr("&And"), this);
    //andNodeAct->setShortcuts(QKeySequence::Undo);
    andNodeAct->setStatusTip(tr("Add an And Node"));
    connect(andNodeAct, SIGNAL(triggered()), this, SLOT(AddAnd()));
    delete node;

    node = NodeFactory::Create(ui->graphicsView,fNOT);
    notNodeAct = new QAction(node->generateIcon(), tr("&Not"), this);
    //notNodeAct->setShortcuts(QKeySequence::Undo);
    notNodeAct->setStatusTip(tr("Add a Not Node"));
    connect(notNodeAct, SIGNAL(triggered()), this, SLOT(AddNot()));
    delete node;

    node = NodeFactory::Create(ui->graphicsView,fFUZZY);
    fuzzyNodeAct = new QAction(node->generateIcon(), tr("&Fuzzy"), this);
    fuzzyNodeAct->setStatusTip(tr("Add a Fuzzy Node"));
    connect(fuzzyNodeAct, SIGNAL(triggered()), this, SLOT(AddFuzzy()));
    delete node;

    node = NodeFactory::Create(ui->graphicsView,fSETUP);
    setupNodeAct = new QAction(node->generateIcon(), tr("&Setup"), this);
    setupNodeAct->setStatusTip(tr("Add a setup node"));
    connect(setupNodeAct, SIGNAL(triggered()), this, SLOT(AddSetup()));
    delete node;

    node = NodeFactory::Create(ui->graphicsView,fPID);
    pidNodeAct = new QAction(node->generateIcon(), tr("&PID"), this);
    pidNodeAct->setStatusTip(tr("Add a PID node"));
    connect(pidNodeAct, SIGNAL(triggered()), this, SLOT(AddPID()));
    delete node;

    node = NodeFactory::Create(ui->graphicsView,fTIMER);
    timerNodeAct = new QAction(node->generateIcon(), tr("&Timer"), this);
    timerNodeAct->setStatusTip(tr("Add a timer node"));
    connect(timerNodeAct, SIGNAL(triggered()), this, SLOT(AddTimer()));
    delete node;

    node = NodeFactory::Create(ui->graphicsView,fDEFINE);
    defineNodeAct = new QAction(node->generateIcon(), tr("&Def"), this);
    defineNodeAct->setStatusTip(tr("Add a timer node"));
    connect(defineNodeAct, SIGNAL(triggered()), this, SLOT(AddDefine()));
    delete node;


}


void MainWindow::on_actionSave_arduino_triggered()
{
    // save as arduino source
QString filename;
        filename = ui->actionSave_arduino->text();
        filename.remove(0,strlen("Save (Arduino) "));
       if (filename.isEmpty())
           return;
       SaveFile(filename,true);
}

void MainWindow::on_Verbose_clicked()
{
    verbose = (ui->Verbose->checkState() == Qt::Checked) || (ui->Verbose->checkState() == Qt::PartiallyChecked);
}
