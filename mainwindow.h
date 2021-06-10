#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <QInputDialog>
#include <QDir>
#include <QMessageBox>
#include <QListIterator>
#include <QPushButton>
#include <QObject>
#include <QDialogButtonBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cards_list_currentTextChanged(const QString &currentText);

    void on_search_line_textChanged(const QString &arg1);

    void on_change_deck_clicked();

    void on_add_deck_clicked();

    void on_remove_deck_clicked();

    void on_add_card_clicked();

    void on_remov_card_clicked();

    void on_actionQuit_triggered();

    void on_start_practise_clicked();

private:
    Ui::MainWindow *ui;

    QHash<QString, QStringList> words;
    QStringList decks_names;
    QHash < QString, QHash < QString, QString > > decks;
    QString current_deck;
    QString current_card;
    QHash<QString, QHash<QString, QPair <int, int>>> stats;

    void load_cards();
    void updateDeck();
    void play();
    void reset();
    void load_stats();
    void removeDeckStats(QString cd);
    void addWordStats(QString cd, QString cw);
    void removeWordStats(QString cd, QString cw);
    void saveStats();
    void practise();
    void addDeckStats();
};
#endif // MAINWINDOW_H
