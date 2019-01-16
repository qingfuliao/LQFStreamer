/****************************************************************************
** Meta object code from reading C++ file 'QTMediaPlayer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../QTMediaPlayer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QTMediaPlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_QTMediaPlayer_t {
    QByteArrayData data[17];
    char stringdata0[279];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QTMediaPlayer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QTMediaPlayer_t qt_meta_stringdata_QTMediaPlayer = {
    {
QT_MOC_LITERAL(0, 0, 13), // "QTMediaPlayer"
QT_MOC_LITERAL(1, 14, 26), // "signalChangePlayButtonIcon"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 25), // "signalChangeTotalDuration"
QT_MOC_LITERAL(4, 68, 26), // "signalChangeProgressSlider"
QT_MOC_LITERAL(5, 95, 8), // "slotStop"
QT_MOC_LITERAL(6, 104, 15), // "slotPlayOrPause"
QT_MOC_LITERAL(7, 120, 16), // "setProgressValue"
QT_MOC_LITERAL(8, 137, 24), // "slotChangePlayButtonIcon"
QT_MOC_LITERAL(9, 162, 14), // "slotChangeTime"
QT_MOC_LITERAL(10, 177, 24), // "slotChangeProgressSlider"
QT_MOC_LITERAL(11, 202, 16), // "slotSeekProgress"
QT_MOC_LITERAL(12, 219, 17), // "slotProgressPress"
QT_MOC_LITERAL(13, 237, 12), // "slotOpenFile"
QT_MOC_LITERAL(14, 250, 8), // "slotSlow"
QT_MOC_LITERAL(15, 259, 8), // "slotFast"
QT_MOC_LITERAL(16, 268, 10) // "slotVolume"

    },
    "QTMediaPlayer\0signalChangePlayButtonIcon\0"
    "\0signalChangeTotalDuration\0"
    "signalChangeProgressSlider\0slotStop\0"
    "slotPlayOrPause\0setProgressValue\0"
    "slotChangePlayButtonIcon\0slotChangeTime\0"
    "slotChangeProgressSlider\0slotSeekProgress\0"
    "slotProgressPress\0slotOpenFile\0slotSlow\0"
    "slotFast\0slotVolume"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QTMediaPlayer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,
       3,    1,   97,    2, 0x06 /* Public */,
       4,    1,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,  103,    2, 0x08 /* Private */,
       6,    0,  104,    2, 0x08 /* Private */,
       7,    1,  105,    2, 0x08 /* Private */,
       8,    1,  108,    2, 0x08 /* Private */,
       9,    1,  111,    2, 0x08 /* Private */,
      10,    1,  114,    2, 0x08 /* Private */,
      11,    1,  117,    2, 0x08 /* Private */,
      11,    0,  120,    2, 0x08 /* Private */,
      12,    0,  121,    2, 0x08 /* Private */,
      13,    0,  122,    2, 0x08 /* Private */,
      14,    0,  123,    2, 0x08 /* Private */,
      15,    0,  124,    2, 0x08 /* Private */,
      16,    1,  125,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void QTMediaPlayer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QTMediaPlayer *_t = static_cast<QTMediaPlayer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->signalChangePlayButtonIcon((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->signalChangeTotalDuration((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->signalChangeProgressSlider((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->slotStop(); break;
        case 4: _t->slotPlayOrPause(); break;
        case 5: _t->setProgressValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->slotChangePlayButtonIcon((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->slotChangeTime((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->slotChangeProgressSlider((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: _t->slotSeekProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->slotSeekProgress(); break;
        case 11: _t->slotProgressPress(); break;
        case 12: _t->slotOpenFile(); break;
        case 13: _t->slotSlow(); break;
        case 14: _t->slotFast(); break;
        case 15: _t->slotVolume((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (QTMediaPlayer::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QTMediaPlayer::signalChangePlayButtonIcon)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (QTMediaPlayer::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QTMediaPlayer::signalChangeTotalDuration)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (QTMediaPlayer::*_t)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&QTMediaPlayer::signalChangeProgressSlider)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject QTMediaPlayer::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QTMediaPlayer.data,
      qt_meta_data_QTMediaPlayer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *QTMediaPlayer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QTMediaPlayer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_QTMediaPlayer.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int QTMediaPlayer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void QTMediaPlayer::signalChangePlayButtonIcon(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTMediaPlayer::signalChangeTotalDuration(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTMediaPlayer::signalChangeProgressSlider(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
