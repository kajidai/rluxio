extern "C" {
#include "ruby.h"
}

#include "luxio/btree.h"

typedef struct {
    Lux::IO::Btree *bt;
}LuxIO;

VALUE rb_luxiobtree_open(VALUE self, VALUE _filename, VALUE _flag)
{
    LuxIO *ptr;
    Data_Get_Struct(self, LuxIO, ptr);

    char *filename = STR2CSTR(_filename);
    int flag = NUM2INT(_flag);

    ptr->bt = new Lux::IO::Btree(Lux::IO::CLUSTER);
    if (ptr->bt->open(filename, Lux::DB_CREAT)){
        return Qtrue;
    }
    return Qnil;
}

VALUE rb_luxiobtree_put(VALUE self, VALUE _key, VALUE _val)
{
    LuxIO *ptr;
    Data_Get_Struct(self, LuxIO, ptr);

    char *key = STR2CSTR(_key);
    char *val = STR2CSTR(_val);

    Lux::IO::data_t lux_key = {key, strlen(key)};
    Lux::IO::data_t lux_val = {val, strlen(val)};
    if (ptr->bt->put(&lux_key, &lux_val)) {
        return Qtrue;
    }
    return Qnil;
}

VALUE rb_luxiobtree_get(VALUE self, VALUE _key)
{
    LuxIO *ptr;
    Data_Get_Struct(self, LuxIO, ptr);

    char *key = STR2CSTR(_key);
    Lux::IO::data_t lux_key = {key, strlen(key)};
    Lux::IO::data_t *lux_val = ptr->bt->get(&lux_key);

    if (lux_val) {
        return rb_str_new2((char *)lux_val->data);
    }
    return Qnil;
}

VALUE rb_luxiobtree_del(VALUE self, VALUE _key)
{
    LuxIO *ptr;
    Data_Get_Struct(self, LuxIO, ptr);

    char *key = STR2CSTR(_key);
    Lux::IO::data_t lux_key = {key, strlen(key)};
    if (ptr->bt->del(&lux_key)) {
        return Qtrue;
    }
    return Qnil;
}

static VALUE luxio_object_delete(LuxIO *ptr)
{
    ptr->bt->close();
    delete ptr->bt;
}

static VALUE luxio_object_alloc(VALUE klass)
{
    LuxIO *ptr = ALLOC(LuxIO);
    return Data_Wrap_Struct(klass, 0, luxio_object_delete, ptr);
}

extern "C" {
    void Init_luxio()
    {
        VALUE klass;
        klass = rb_define_class("LuxIOBtree" ,rb_cObject);
        rb_define_singleton_method(klass, "new", reinterpret_cast<VALUE(*)(...)>(luxio_object_alloc), 0); 
        rb_define_method(klass, "open", reinterpret_cast<VALUE(*)(...)>(rb_luxiobtree_open), 2);
        rb_define_method(klass, "put", reinterpret_cast<VALUE(*)(...)>(rb_luxiobtree_put), 2);
        rb_define_method(klass, "get", reinterpret_cast<VALUE(*)(...)>(rb_luxiobtree_get), 1);
        rb_define_method(klass, "del", reinterpret_cast<VALUE(*)(...)>(rb_luxiobtree_del), 1);

        rb_define_const(klass, "LUX_DB_RDONLY", INT2NUM(Lux::DB_RDONLY));
        rb_define_const(klass, "LUX_DB_RDWR", INT2NUM(Lux::DB_RDWR));
        rb_define_const(klass, "LUX_DB_CREAT", INT2NUM(Lux::DB_CREAT));
        rb_define_const(klass, "LUX_DB_TRUNC", INT2NUM(Lux::DB_TRUNC));
    }
}

