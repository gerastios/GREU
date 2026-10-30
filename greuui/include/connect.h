#pragma once
#include <QNetworkReply>
#include <QObject>
#include <QProcess>
#include "greucore/include/greuGlobal.h"

class QAbstractButton;
class QAbstractItemView;
class QCheckBox;
class QComboBox;
class QDateEdit;
class QDateTimeEdit;
class QFileSystemModel;
class QLabel;
class QLineEdit;
class QMediaPlayer;
class QSlider;
class QSpinBox;
class QTabWidget;
class QTextBrowser;
class QAction;

namespace greu::ui {

QMetaObject::Connection GREU_PUBLIC(UI) connect(QAbstractButton *, std::function<void()>);
QMetaObject::Connection GREU_PUBLIC(UI) connect(QAction *, std::function<void()>);
QMetaObject::Connection GREU_PUBLIC(UI) connect(QLabel *, std::function<void(QString const &)>);
QMetaObject::Connection GREU_PUBLIC(UI) connect(QSpinBox *, std::function<void(int)>);
QMetaObject::Connection GREU_PUBLIC(UI) connect(QTextBrowser *, std::function<void(QUrl const &)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectClicked(QAbstractItemView *, std::function<void(QModelIndex const &index)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectCustomCtxMenu(QWidget *, std::function<void(QPoint const &)>);

QMetaObject::Connection GREU_PUBLIC(UI) connectCurrent(QTabWidget *, std::function<void(int)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectDateChanged(QDateEdit *, std::function<void(QDate const &)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectDateTimeChanged(QDateTimeEdit *, std::function<void(QDateTime const &)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectDestroyed(QObject *, std::function<void(QObject *)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectDirectoryLoaded(QFileSystemModel *, std::function<void(QString const &)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectDoubleClicked(QAbstractItemView *, std::function<void(QModelIndex const &index)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectDurationChanged(QMediaPlayer *, std::function<void(qint64)>);

QMetaObject::Connection GREU_PUBLIC(UI) connectEditTextChanged(QComboBox *, std::function<void(QString const &)>);

QMetaObject::Connection GREU_PUBLIC(UI) connectFinished(QProcess *, std::function<void()>);
QMetaObject::Connection GREU_PUBLIC(UI) connectFinished(QLineEdit *, std::function<void()>);
QMetaObject::Connection GREU_PUBLIC(UI) connectFinished(QNetworkReply*, std::function<void()>);

QMetaObject::Connection GREU_PUBLIC(UI) connectIndex(QComboBox *, std::function<void(int)>);

QMetaObject::Connection GREU_PUBLIC(UI) connectOnError(QProcess *, std::function<void()>);
QMetaObject::Connection GREU_PUBLIC(UI) connectOnError(QNetworkReply *, std::function<void(QNetworkReply::NetworkError)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectPositionChanged(QMediaPlayer *, std::function<void(qint64)>);

QMetaObject::Connection GREU_PUBLIC(UI) connectReadError(QProcess *, std::function<void()>);

QMetaObject::Connection GREU_PUBLIC(UI) connectReadOutput(QProcess *, std::function<void()>);

QMetaObject::Connection GREU_PUBLIC(UI) connectReleased(QSlider *, std::function<void()>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectState(QCheckBox *, std::function<void(Qt::CheckState)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectTextChanged(QLineEdit *, std::function<void(QString const &)>);
QMetaObject::Connection GREU_PUBLIC(UI) connectTextChanged(QComboBox *, std::function<void()>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectTextEdited(QLineEdit *, std::function<void(QString const &)>);

QMetaObject::Connection GREU_PUBLIC(UI)
    connectStateChanged(QProcess *, std::function<void(QProcess::ProcessState)>);

} // namespace greu::ui
