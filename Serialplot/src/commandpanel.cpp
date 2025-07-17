/*
  Copyright © 2025 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QByteArray>
#include <QtDebug>

#include "commandpanel.h"
#include "ui_commandpanel.h"
#include "setting_defines.h"



CommandPanel::CommandPanel(QSerialPort* port, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandPanel),
    _menu(tr("&Key_Insert")), _newCommandAction(tr("&请输入密钥"), this)
{
    serialPort = port;

    ui->setupUi(this);
    QList<QTextEdit*> textEdites =
    {
        ui->textEdit,
        ui->textEdit_2,
        ui->textEdit_3,
        ui->textEdit_4,
        ui->textEdit_5,
        ui->textEdit_6,
    };
    QList<QComboBox*> comboBoxes = {
        ui->comboBox,
        ui->comboBox_2,
        ui->comboBox_3,
        ui->comboBox_4,
        ui->comboBox_5,
        ui->comboBox_6
    };
    QList<QPushButton*> buttons =
    {
            ui->pushButton,
            ui->pushButton_2,
            ui->pushButton_3,
            ui->pushButton_4,
            ui->pushButton_5,
            ui->pushButton_6,
    };
    QList<QLabel*> labels =
    {
        ui->label,
        ui->label_2,
        ui->label_3,
        ui->label_4,
        ui->label_5,
        ui->label_6,
    };

    QString style = "border: 2px solid black; padding: 5px;";
    for (QTextEdit* edit : textEdites)
    {
        edit->setStyleSheet(style);
    }

    // 向每个 ComboBox 添加 0 到 100 的整数项
    for (int i = 0; i <= 100; ++i)
    {
        QString str = QString::number(i);
        for (QComboBox* cb : comboBoxes)
        {
            cb->addItem(str);
        }
    }
    QStringList stringButton = { "on", "off", "Press","Press_One","Flow","Flow_One"};
    for (int i = 0; i < buttons.size(); ++i)
    {
        buttons[i]->setObjectName(stringButton[i]);
        connect(buttons[i], &QPushButton::clicked, this, &CommandPanel::Serial_send);
    }
    for(int i=0;i<labels.size();++i)
    {
        labels[i]->setText(stringButton[i]);
        labels[i]->setAlignment(Qt::AlignCenter);
        labels[i]->setStyleSheet(style);
    }

    _menu.addAction(&_newCommandAction);
    _menu.addSeparator();

    command_name_counter = 0;
}

CommandPanel::~CommandPanel()
{
    commands.clear(); // UI will 'delete' actual objects
    delete ui;
}

#if 0
CommandWidget* CommandPanel::newCommand()
{
    auto command = new CommandWidget();
    command_name_counter++;
    command->setName(tr("Command ") + QString::number(command_name_counter));
    command->setFocusToEdit();
    connect(command, &CommandWidget::sendCommand, this, &CommandPanel::sendCommand);
    connect(command, &CommandWidget::focusRequested, this, &CommandPanel::focusRequested);
    _menu.addAction(command->sendAction());

    // add to command list and remove on destroy
    commands << command;
    connect(command, &QObject::destroyed, [this](QObject* obj)
    {
        commands.removeOne(static_cast<CommandWidget*>(obj));
        reAssignShortcuts();
    });

    reAssignShortcuts();
    return command;
}
#endif
void CommandPanel::reAssignShortcuts()
{
    // can assign shortcuts to first 12 commands
    for (int i = 0; i < std::min(12, (int)commands.size()); i++)
    {
        auto cmd = commands[i];
        cmd->sendAction()->setShortcut(QKeySequence(Qt::Key_F1 + i));
    }
}

void CommandPanel::Serial_send(void)
{
    if (!serialPort->isOpen())
    {
        qCritical() << "Port is not open!";
        return;
    }
    QObject *obj = sender();  // 获取信号发出者
    if (obj->objectName()=="ON")
    {
        if (serialPort->write("1111") < 0)
        {
            qCritical() << "Send command failed!";
        }
    }

}

#if 0
void CommandPanel::sendCommand(QByteArray command)
{
    if (!serialPort->isOpen())
    {
        qCritical() << "Port is not open!";


        return;
    }

    if (serialPort->write(command) < 0)
    {
        qCritical() << "Send command failed!";
    }
}
#endif

QMenu* CommandPanel::menu()
{
    return &_menu;
}

QAction* CommandPanel::newCommandAction()
{
    return &_newCommandAction;
}

unsigned CommandPanel::numOfCommands()
{
    return commands.size();
}
#if 0
void CommandPanel::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Commands);
    settings->beginWriteArray(SG_Commands_Command);
    for (int i = 0; i < commands.size(); i ++)
    {
        settings->setArrayIndex(i);
        auto command = commands[i];
        settings->setValue(SG_Commands_Name, command->name());
        settings->setValue(SG_Commands_Type, command->isASCIIMode() ? "ascii" : "hex");
        settings->setValue(SG_Commands_Data, command->commandText());
    }
    settings->endArray();
    settings->endGroup();
}

void CommandPanel::loadSettings(QSettings* settings)
{
    // clear all commands
    while (commands.size())
    {
        auto command = commands.takeLast();
        command->disconnect();
        delete command;
    }

    // load commands
    settings->beginGroup(SettingGroup_Commands);
    unsigned size = settings->beginReadArray(SG_Commands_Command);

    for (unsigned i = 0; i < size; i ++)
    {
        settings->setArrayIndex(i);
        auto command = newCommand();

        // load command name
        QString name = settings->value(SG_Commands_Name, "").toString();
        if (!name.isEmpty()) command->setName(name);

        // Important: type should be set before command data for correct validation
        QString type = settings->value(SG_Commands_Type, "").toString();
        if (type == "ascii")
        {
            command->setASCIIMode(true);
        }
        else if (type == "hex")
        {
            command->setASCIIMode(false);
        } // else unchanged

        // load command data
        command->setCommandText(settings->value(SG_Commands_Data, "").toString());
    }

    settings->endArray();
    settings->endGroup();
}
#endif
