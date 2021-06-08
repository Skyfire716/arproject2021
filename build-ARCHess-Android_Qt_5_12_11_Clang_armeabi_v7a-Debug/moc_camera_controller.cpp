/****************************************************************************
** Meta object code from reading C++ file 'camera_controller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../ARChess/camera_controller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camera_controller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_camera_worker_t {
    QByteArrayData data[11];
    char stringdata0[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_camera_worker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_camera_worker_t qt_meta_stringdata_camera_worker = {
    {
QT_MOC_LITERAL(0, 0, 13), // "camera_worker"
QT_MOC_LITERAL(1, 14, 11), // "image_ready"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 3), // "img"
QT_MOC_LITERAL(4, 31, 8), // "finished"
QT_MOC_LITERAL(5, 40, 15), // "camera_detected"
QT_MOC_LITERAL(6, 56, 8), // "cam_name"
QT_MOC_LITERAL(7, 65, 13), // "capture_video"
QT_MOC_LITERAL(8, 79, 13), // "change_camera"
QT_MOC_LITERAL(9, 93, 8), // "cv_index"
QT_MOC_LITERAL(10, 102, 3) // "run"

    },
    "camera_worker\0image_ready\0\0img\0finished\0"
    "camera_detected\0cam_name\0capture_video\0"
    "change_camera\0cv_index\0run"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_camera_worker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    1,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   51,    2, 0x0a /* Public */,
       8,    1,   52,    2, 0x0a /* Public */,
      10,    0,   55,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QPixmap,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void,

       0        // eod
};

void camera_worker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<camera_worker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->image_ready((*reinterpret_cast< QPixmap(*)>(_a[1]))); break;
        case 1: _t->finished(); break;
        case 2: _t->camera_detected((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->capture_video(); break;
        case 4: _t->change_camera((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->run(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (camera_worker::*)(QPixmap );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&camera_worker::image_ready)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (camera_worker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&camera_worker::finished)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (camera_worker::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&camera_worker::camera_detected)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject camera_worker::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_camera_worker.data,
    qt_meta_data_camera_worker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *camera_worker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *camera_worker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_camera_worker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int camera_worker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void camera_worker::image_ready(QPixmap _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void camera_worker::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void camera_worker::camera_detected(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_camera_controller_t {
    QByteArrayData data[5];
    char stringdata0[51];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_camera_controller_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_camera_controller_t qt_meta_stringdata_camera_controller = {
    {
QT_MOC_LITERAL(0, 0, 17), // "camera_controller"
QT_MOC_LITERAL(1, 18, 4), // "init"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 13), // "start_capture"
QT_MOC_LITERAL(4, 38, 12) // "stop_capture"

    },
    "camera_controller\0init\0\0start_capture\0"
    "stop_capture"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_camera_controller[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    0,   30,    2, 0x0a /* Public */,
       4,    0,   31,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void camera_controller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<camera_controller *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->init(); break;
        case 1: _t->start_capture(); break;
        case 2: _t->stop_capture(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject camera_controller::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_camera_controller.data,
    qt_meta_data_camera_controller,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *camera_controller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *camera_controller::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_camera_controller.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int camera_controller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
