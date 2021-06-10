#include "practise.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <bits/stdc++.h>

using namespace std;

void MainWindow::load_stats()
{
    QFile inFile("stats");
    if (!inFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    QTextStream in(&inFile);
    QString line;
    QString deck_name;
    QString word;
    int trueAns;
    int allAns;
    QHash<QString, QPair<int, int>> cur;
    while (!in.atEnd())
    {
        line = in.readLine();
        if (!line.isEmpty() && line[0].isSpace())
        {
            if (!deck_name.isEmpty())
            {
                if (!word.isEmpty())
                {
                    cur.insert(word, qMakePair(trueAns, allAns));
                }
                word.clear();
                stats.insert(deck_name, cur);
                cur.clear();
            }
            deck_name = line.simplified();
        }
        else if (!line.isEmpty())
        {
            in >> trueAns >> allAns;
            if (!word.isEmpty())
            {
                cur.insert(word, qMakePair(trueAns, allAns));
            }
            word = line.simplified();
        }
    }
    if (!word.isEmpty())
    {
        cur.insert(word, qMakePair(trueAns, allAns));
    }
    if (!deck_name.isEmpty())
    {
        word.clear();
        stats.insert(deck_name, cur);
        cur.clear();
    }
}

void MainWindow::practise()
{
    QMessageBox msg;
    msg.setText("Start practise?");
    QPushButton *playButton = msg.addButton(tr("Start"), QMessageBox::AcceptRole);
    QPushButton *resetButton = msg.addButton(tr("Reset progress"), QMessageBox::RejectRole);
    msg.setStandardButtons(QMessageBox::Cancel);
    int ret = msg.exec();

    if (msg.clickedButton() == playButton)
    {
        play();
    }
    else if (msg.clickedButton() == resetButton)
    {
        reset();
    }
}

void MainWindow::removeDeckStats(QString cd)
{
    QHash<QString, QHash<QString, QPair <int, int>>>::iterator it = stats.begin();
    while (it != stats.end())
    {
        if (it.key() == cd)
        {
            stats.erase(it);
            break;
        }
        it++;
    }
}

void MainWindow::addWordStats(QString cd, QString cw)
{
    QHash<QString, QHash<QString, QPair <int, int>>>::iterator it = stats.begin();
    while (it != stats.end())
    {
        if (it.key() == cd)
        {
            stats[it.key()].insert(cw, qMakePair(0, 0));
            break;
        }
        it++;
    }
}

void MainWindow::removeWordStats(QString cd, QString cw)
{
    QHash<QString, QHash<QString, QPair <int, int>>>::iterator it = stats.begin();
    while (it != stats.end())
    {
        if (it.key() == cd)
        {
            QHash<QString, QPair <int, int>>::iterator i = stats[it.key()].begin();
            while (i != stats[it.key()].end())
            {
                if (i.key() == cw)
                {
                    stats[it.key()].erase(i);
                    break;
                }
                i++;
            }
            break;
        }
        it++;
    }
}

void MainWindow::addDeckStats()
{
    QHash<QString, QPair<int, int>> cur_stats;
    stats.insert(current_deck, cur_stats);
}

void MainWindow::saveStats()
{
    QFile outFile("stats");
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&outFile);
    QHash<QString, QHash<QString, QPair <int, int>>>::iterator it = stats.begin();
    while (it != stats.end())
    {
        out << " " << it.key()  << "\n";
        QHash<QString, QPair <int, int>>::iterator i = stats[it.key()].begin();
        while (i != stats[it.key()].end())
        {
            out << i.key() << "\n" << stats[it.key()][i.key()].first << " " << stats[it.key()][i.key()].second << "\n";
            i++;
        }
        it++;
    }
}

void MainWindow::play()
{
    saveStats();
    if (current_deck.isEmpty())
    {
        QMessageBox er;
        er.setText("Chose deck first");
        int rt = er.exec();
        return;
    }
    if (decks[current_deck].empty())
    {
        QMessageBox er;
        er.setText("This deck has not got any cards");
        int rt = er.exec();
        return;
    }
    vector<pair<double, QString>> lst;
    QHash<QString, QPair <qint32, qint32>>::iterator it = stats[current_deck].begin();
    while (it != stats[current_deck].end())
    {
        double id;
        if ((*it).second == 0)
            id = 0;
        else if ((*it).first == 0)
            id = (*it).second*(-1.0);
        else
            id = (double)(*it).first/(double)(*it).second;
        lst.push_back({id, it.key()});
        it++;
    }
    int correct = 0;
    int i = 0;
    sort(lst.begin(), lst.end());
    while (1)
    {
        if (i == (int)lst.size())
            break;
        if (i == 10)
            break;
        bool ok;
        QString text = QInputDialog::getText(this, tr("Type translation:"), lst[i].second, QLineEdit::Normal, QDir::home().dirName(), &ok);
        if (ok && !text.isEmpty())
        {
            text = text.simplified();
            if (text == decks[current_deck][lst[i].second])
            {
                stats[current_deck][lst[i].second].first++;
                stats[current_deck][lst[i].second].second++;
                correct++;
            }
            else
            {
                stats[current_deck][lst[i].second].second++;
            }
        }
        else if (!ok)
            return;
        i++;
    }
    QMessageBox msg;
    QString show = QString("Your result is %1 / %2 ").arg(correct).arg(i);
    msg.setText(show);
    int ret = msg.exec();
}

void MainWindow::reset()
{
    if (current_deck.isEmpty())
        return;
    QMessageBox msg;
    msg.setText("Do you really want to reset you progress?");
    msg.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    int ret = msg.exec();
    switch (ret)
    {
        case QMessageBox::No:
            return;
        default:
        {
            QHash<QString, QPair<int, int>>::iterator it = stats[current_deck].begin();
            while (it != stats[current_deck].end())
            {
                (*it).first = 0;
                (*it).second = 0;
                it++;
            }
        }
    }
}
