#include "connect.h"
#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QFileSystemModel>
#include <QLabel>
#include <QLineEdit>
#include <QMediaPlayer>
#include <QProcess>
#include <QPushButton>
#include <QSpinBox>
#include <QTextBrowser>

using namespace std;

namespace greu::ui {

#define IMPL(FUNC) \
    assert(o); \
    assert(f); \
    return QObject::connect(o, &FUNC, f)

QMetaObject::Connection connect(QAbstractButton *o, function<void()> f) {
    IMPL(QAbstractButton::clicked);
}

QMetaObject::Connection connect(QAction *o, function<void()> f) {
    IMPL(QAction::triggered);
}

QMetaObject::Connection connectState(QCheckBox *o, function<void(Qt::CheckState)> f) {
    IMPL(QCheckBox::checkStateChanged);
}

QMetaObject::Connection connectTextEdited(QLineEdit *o, function<void(QString const &)> f) {
    IMPL(QLineEdit::textEdited);
}

QMetaObject::Connection connectTextChanged(QLineEdit *o, function<void(QString const &)> f) {
    IMPL(QLineEdit::textChanged);
}

QMetaObject::Connection connectEditTextChanged(QComboBox *o, function<void(QString const &)> f) {
    IMPL(QComboBox::editTextChanged);
}

QMetaObject::Connection connectDateChanged(QDateEdit *o, function<void(QDate const &)> f) {
    IMPL(QDateEdit::dateChanged);
}

QMetaObject::Connection connectDateTimeChanged(QDateTimeEdit *o,
                                               function<void(QDateTime const &)> f) {
    IMPL(QDateTimeEdit::dateTimeChanged);
}

QMetaObject::Connection connectDestroyed(QObject *o,
                                               function<void(QObject *)> f) {
    IMPL(QObject::destroyed);
}

QMetaObject::Connection connect(QSpinBox *o, function<void(int)> f) {
    IMPL(QSpinBox::valueChanged);
}

QMetaObject::Connection connect(QTextBrowser *o, function<void(QUrl const &)> f) {
    IMPL(QTextBrowser::anchorClicked);
}

QMetaObject::Connection connect(QLabel *o, function<void(QString const &)> f) {
    IMPL(QLabel::linkActivated);
}

QMetaObject::Connection connectReadOutput(QProcess *o, function<void()> f) {
    IMPL(QProcess::readyReadStandardOutput);
}

QMetaObject::Connection connectOnError(QProcess *o, function<void()> f) {
    IMPL(QProcess::errorOccurred);
}

QMetaObject::Connection connectOnError(QNetworkReply *o,
                                       std::function<void(QNetworkReply::NetworkError)> f) {
    IMPL(QNetworkReply::errorOccurred);
}

QMetaObject::Connection connectFinished(QProcess *o, function<void()> f) {
    IMPL(QProcess::finished);
}

QMetaObject::Connection connectReadError(QProcess *o, function<void()> f) {
    IMPL(QProcess::readyReadStandardError);
}

QMetaObject::Connection connectStateChanged(QProcess *o,
                                            function<void((QProcess::ProcessState))> f) {
    IMPL(QProcess::stateChanged);
}

QMetaObject::Connection connectIndex(QComboBox *o, function<void(int)> f) {
    IMPL(QComboBox::currentIndexChanged);
}

QMetaObject::Connection connectCustomCtxMenu(QWidget *o, function<void(QPoint const &)> f) {
    IMPL(QWidget::customContextMenuRequested);
}

QMetaObject::Connection connectDoubleClicked(QAbstractItemView *o,
                                             function<void(QModelIndex const &)> f) {
    IMPL(QAbstractItemView::doubleClicked);
}

QMetaObject::Connection connectClicked(QAbstractItemView *o, function<void(QModelIndex const &)> f) {
    IMPL(QAbstractItemView::clicked);
}

QMetaObject::Connection connectCurrent(QTabWidget *o, function<void(int)> f) {
    IMPL(QTabWidget::currentChanged);
}

QMetaObject::Connection connectDirectoryLoaded(QFileSystemModel *o,
                                               function<void(QString const &)> f) {
    IMPL(QFileSystemModel::directoryLoaded);
}

QMetaObject::Connection connectFinished(QLineEdit *o, function<void()> f) {
    IMPL(QLineEdit::editingFinished);
}

QMetaObject::Connection connectTextChanged(QComboBox *o, function<void()> f) {
    IMPL(QComboBox::currentTextChanged);
}

QMetaObject::Connection connectFinished(QNetworkReply *o, function<void()> f) {
    IMPL(QNetworkReply::finished);
}

QMetaObject::Connection connectDurationChanged(QMediaPlayer *o, function<void(qint64)> f) {
    IMPL(QMediaPlayer::durationChanged);
}

QMetaObject::Connection connectPositionChanged(QMediaPlayer *o, function<void(qint64)> f) {
    IMPL(QMediaPlayer::positionChanged);
}

QMetaObject::Connection connectReleased(QSlider *o, std::function<void()> f) {
    IMPL(QSlider::sliderReleased);
}

} // namespace greu::ui
