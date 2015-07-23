/****************************************************************************
** Meta object code from reading C++ file 'shape.h'
**
** Created: Sun Jun 22 09:46:53 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "shape.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'shape.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Shape[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
      24,    6,    6,    6, 0x0a,
      59,   46,    6,    6, 0x0a,
      85,    6,    6,    6, 0x0a,
     107,    6,    6,    6, 0x0a,
     143,  139,    6,    6, 0x08,
     165,  139,    6,    6, 0x08,
     186,    6,    6,    6, 0x08,
     212,    6,    6,    6, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Shape[] = {
    "Shape\0\0refreshChanges()\0enableContourFilter()\0"
    "nrOfCounturs\0setNumberOfCountours(int)\0"
    "enableOutlineFilter()\0"
    "refreshParametersOfGivenShape()\0val\0"
    "setStartRange(double)\0setStopRange(double)\0"
    "setSampleFuncDimmension()\0setColours()\0"
};

void Shape::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Shape *_t = static_cast<Shape *>(_o);
        switch (_id) {
        case 0: _t->refreshChanges(); break;
        case 1: _t->enableContourFilter(); break;
        case 2: _t->setNumberOfCountours((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->enableOutlineFilter(); break;
        case 4: _t->refreshParametersOfGivenShape(); break;
        case 5: _t->setStartRange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->setStopRange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->setSampleFuncDimmension(); break;
        case 8: _t->setColours(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Shape::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Shape::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Shape,
      qt_meta_data_Shape, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Shape::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Shape::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Shape::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Shape))
        return static_cast<void*>(const_cast< Shape*>(this));
    return QObject::qt_metacast(_clname);
}

int Shape::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void Shape::refreshChanges()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
