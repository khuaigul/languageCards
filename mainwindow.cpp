#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    load_cards();
    ui->deck_name->insertPlainText(current_deck);
    ui->cards_list->addItems(words[current_deck]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load_cards()
{
    QFile inFile("cards");
    if (!inFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    QTextStream in(&inFile);
    QString line;
    QString deck_name;
    QString word;
    QString translation;
    qint64 id = 0;
    QHash <QString, QString> cur_deck;
    while (!in.atEnd())
    {
        line = in.readLine();
        if (!line.isEmpty() && line[0].isSpace())
        {
            if (!deck_name.isEmpty())
            {
                if (!word.isEmpty())
                {
                    words[deck_name].append(word);
                    cur_deck.insert(word, translation);
                }
                word.clear();
                decks.insert(deck_name, cur_deck);
                cur_deck.clear();
                decks_names.append(deck_name);
            }
            deck_name = line.simplified();
            id = 0;
        }
        else
        {
            if (id == 0)
            {
                if (!word.isEmpty())
                {
                    words[deck_name].append(word);
                    cur_deck.insert(word, translation);
                }
                word = line.simplified();
            }
            else
            {
                translation = line.simplified();
            }
            id++;
            id = id % 2;
        }
    }
    if (!word.isEmpty())
    {
        words[deck_name].append(word);
        cur_deck.insert(word, translation);
    }
    if (!deck_name.isEmpty())
    {
        decks.insert(deck_name, cur_deck);
        decks_names.append(deck_name);
        cur_deck.clear();
    }
    if (!decks_names.empty())
    {
        current_deck = decks_names[0];
    }
    inFile.close();
    load_stats();
}


void MainWindow::on_cards_list_currentTextChanged(const QString &currentText)
{
    ui->show_card->setPlainText(currentText + "\n" + decks[current_deck][currentText]);
    current_card = currentText;
}

void MainWindow::on_search_line_textChanged(const QString &arg1)
{
    QRegExp rx(arg1);

    ui->cards_list->clear();
    ui->cards_list->addItems(words[current_deck].filter(rx));
}

void MainWindow::on_change_deck_clicked()
{
    if (decks.empty())
    {
        QMessageBox msg;
        msg.setText("You have not got any decks yet");
        int ret = msg.exec();
        return;
    }
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Choose deck"), tr("Deck:"), decks_names, 0, false, &ok);
    item = item.simplified();
    if (ok && !item.isEmpty())
    {
        current_deck = item;
        updateDeck();
    }
}

void MainWindow::updateDeck()
{
    ui->search_line->clear();
    ui->deck_name->setPlainText(current_deck);
    ui->cards_list->clear();
    ui->cards_list->addItems(words[current_deck]);
    ui->show_card->clear();
    if(!current_card.isEmpty() && !current_deck.isEmpty())
        ui->show_card->setPlainText(current_card + "\n" + decks[current_deck][current_card]);
}

void MainWindow::on_add_deck_clicked()
{
    QHash<QString, QString> cur;
    bool ok;
    QString newDeck = QInputDialog::getText(this, tr("Add a new deck"), tr("New deck name:"), QLineEdit::Normal, QDir::home().dirName(), &ok);
    newDeck = newDeck.simplified();
    if (ok && !newDeck.isEmpty())
    {
        if (decks.find(newDeck) != decks.end())
        {
            QMessageBox msg;
            msg.setText("This name already exists");
            msg.setStandardButtons(QMessageBox::Ok);
            int ret = msg.exec();
            return;
        }
        decks.insert(newDeck, cur);
        current_deck = newDeck;
        decks_names.append(newDeck);
        updateDeck();
    }
    addDeckStats();
}

void MainWindow::on_remove_deck_clicked()
{
    if (current_deck.isEmpty())
        return;
    QMessageBox msg;
    msg.setText("Do you want to remove current deck?");
    msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    int ret = msg.exec();
    switch(ret)
    {
        case QMessageBox::Cancel:
            break;
        default:
        {
            QHash<QString, QHash<QString, QString>>::iterator i = decks.begin();
            while (i != decks.end())
            {
                if (i.key() == current_deck)
                {
                    decks.erase(i);
                    break;
                }
                i++;
            }
            QHash<QString, QStringList>::iterator j = words.begin();
            while (j != words.end())
            {
                if (j.key() == current_deck)
                {
                    words.erase(j);
                    break;
                }
                j++;
            }
            decks_names.removeAll(current_deck);
            removeDeckStats(current_deck);
            current_deck.clear();
            if(!decks_names.isEmpty())
                current_deck = decks_names[0];
            updateDeck();
        }
    }
}

void MainWindow::on_add_card_clicked()
{
    if (current_deck.isEmpty())
    {
        qDebug() << "empty";
        QMessageBox noDeck;
        noDeck.setText("Chose deck first");
        noDeck.setStandardButtons(QMessageBox::Ok);
        int dret = noDeck.exec();
        return;
    }
    bool ok;
    QString newWord = QInputDialog::getText(this, tr("Add a new card"), tr("Enter a word:"), QLineEdit::Normal, QDir::home().dirName(), &ok);
    newWord = newWord.simplified();
    if (ok && !newWord.isEmpty())
    {
        if(decks[current_deck].find(newWord) != decks[current_deck].end())
        {
            QMessageBox msg;
            msg.setText("This word already exists");
            msg.setStandardButtons(QMessageBox::Ok);
            int ret = msg.exec();
            return;
        }
        bool ok1;
        QString newTranslation = QInputDialog::getText(this, tr("Add a new card"), tr("Enter a translation:"), QLineEdit::Normal, QDir::home().dirName(), &ok1);
        newTranslation = newTranslation.simplified();
        if (ok1 && !newTranslation.isEmpty())
        {
            decks[current_deck].insert(newWord, newTranslation);
            words[current_deck].append(newWord);
            updateDeck();
        }
        else if (newTranslation.isEmpty())
        {
            QMessageBox msg;
            msg.setText("Nothing has been added");
            msg.setStandardButtons(QMessageBox::Ok);
            int ret = msg.exec();
        }
    }
    addWordStats(current_deck, newWord);
}


void MainWindow::on_remov_card_clicked()
{
    if (current_card.isEmpty())
        return;
    QMessageBox msg;
    msg.setText("Do you want to remove current card?");
    msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    int ret = msg.exec();

    switch(ret)
    {
        case QMessageBox::Cancel:
            break;
        default:
        {
            QHash<QString, QString>::iterator it = decks[current_deck].begin();
            while (it != decks[current_deck].end())
            {
                if (it == decks[current_deck].end())
                    break;
                if (it.key() == current_card)
                {
                    decks[current_deck].erase(it);
                    break;
                }
                it++;
            }
            removeWordStats(current_deck, current_card);
            words[current_deck].removeAll(current_card);
            current_card.clear();
            updateDeck();
        }
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QMessageBox msg;
    msg.setText("Save all changes?");
    msg.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    int ret = msg.exec();
    switch(ret)
    {
        case QMessageBox::No:
            close();
        default:
        {
            QFile outFile("cards");
            if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
                return;
            QTextStream out(&outFile);
            QHash<QString, QHash<QString, QString>>::iterator i = decks.begin();
            while (i != decks.end())
            {
                out << " " << i.key() << "\n";
                current_deck = i.key();
                QHash<QString, QString>::iterator j = decks[current_deck].begin();
                while (j != decks[current_deck].end())
                {
                    out << j.key() << "\n" << *j << "\n";
                    j++;
                }
                i++;
            }
            saveStats();
            outFile.close();
            close();
        }
    }
}

void MainWindow::on_start_practise_clicked()
{
    practise();
}
