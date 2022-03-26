#include "MainWindow.h"

#include <QDebug>
#include <QRegularExpression>

#include "ui_MainWindow.h"

//#define MAX_WORD_PER_LINE 12
//#define MAX_LINE_PER_PAGE 5

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pushButton_clicked()
{
    QString Contain = ui->textEdit_Source->toPlainText();
    QString Page_Contain;
    int     Line_Count        = 0;
    int     Last_Position     = 0;
    int     MAX_WORD_PER_LINE = ui->spinBox_Char->value();
    int     MAX_LINE_PER_PAGE = ui->spinBox_Line->value();

    ui->textEdit_Target->clear();

    // remove page number, \n, [space]
    Contain.remove(QRegularExpression("\\(p[0-9]*\\)"));
    Contain.remove(QRegularExpression("\\n"));
    Contain.replace(QRegularExpression("([a-zA-Z].*?)【"), "【");
    Contain = Contain.simplified();
    Contain.remove(" ");

    // convert "—" to "－", "……" to "～"
    Contain.replace(QChar(0x2015), QChar(0xff0d));
    Contain.replace("……", "～");

    qDebug() << Contain.contains(QChar(0xff0d));

    while (Contain.size())
    {
        Page_Contain.clear();
        Line_Count = 0;

        while (Contain.size() > MAX_WORD_PER_LINE)
        {
            Line_Count++;
            QString Line = Contain.left(MAX_WORD_PER_LINE);
            Contain      = Contain.mid(MAX_WORD_PER_LINE);

            Page_Contain.append(Line);

            if (Line_Count >= MAX_LINE_PER_PAGE)
                break;
            else
            {
                Page_Contain.append("\\n");
                if(ui->checkBox_Double_Enter->isChecked())
                    Page_Contain.append("\\n");
            }
        }

        // remain message
        if (Line_Count != MAX_LINE_PER_PAGE && !Contain.isEmpty())
        {
            Page_Contain.append(Contain);
            Contain.clear();
        }

        // page full
        if (Line_Count == MAX_LINE_PER_PAGE && ui->checkBox_Cut_By_char->isChecked() ==false)
        {
            Last_Position = Page_Contain.lastIndexOf(
                QRegularExpression("[，。？！：、「」]"));
            Contain.prepend(
                Page_Contain.mid(Last_Position + 1).replace("\\n", ""));
            Page_Contain = Page_Contain.left(Last_Position + 1);
        }

        // convert "【" to "{\rD2}【"
        // convert "】" to "】{\r}"
        Page_Contain.replace("【", "{\\rD2}【");
        Page_Contain.replace("】", "】{\\r}");

        ui->textEdit_Target->append(Page_Contain);
    }
}

void MainWindow::on_pushButton_Clean_clicked()
{
    ui->textEdit_Source->setText("");
}
