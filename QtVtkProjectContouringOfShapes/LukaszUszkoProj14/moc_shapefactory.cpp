/****************************************************************************
** Meta object code from reading C++ file 'shapefactory.h'
**
** Created: Sun Jun 22 09:46:58 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "shapefactory.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shapefactory.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ShapeFactory[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ShapeFactory[] = {
    "ShapeFactory\0\0shapeType\0"
    "retrieveProperShape(QString)\0"
    "sendSignalToGetShape()\0"
};

void ShapeFactory::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ShapeFactory *_t = static_cast<ShapeFactory *>(_o);
        switch (_id) {
        case 0: _t->retrieveProperShape((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->sendSignalToGetShape(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ShapeFactory::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ShapeFactory::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ShapeFactory,
      qt_meta_data_ShapeFactory, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ShapeFactory::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ShapeFactory::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ShapeFactory::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ShapeFactory))
        return static_cast<void*>(const_cast< ShapeFactory*>(this));
    return QObject::qt_metacast(_clname);
}

int ShapeFactory::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ShapeFactory::retrieveProperShape(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
