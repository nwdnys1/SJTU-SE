package org.example.component;

import com.fasterxml.jackson.annotation.JsonTypeInfo;
import com.fasterxml.jackson.databind.DatabindContext;
import com.fasterxml.jackson.databind.JavaType;
import com.fasterxml.jackson.databind.jsontype.impl.TypeIdResolverBase;
import org.hibernate.collection.spi.*;

import java.io.IOException;
import java.lang.reflect.Array;
import java.util.*;

class HibernateCollectionIdResolver extends TypeIdResolverBase {

    public HibernateCollectionIdResolver() {
    }

    @Override
    public String idFromValue(Object value) {
        //translate from HibernanteCollection class to JDK collection class
        if (value instanceof PersistentArrayHolder) {
            return Array.class.getName();
        } else if (value instanceof PersistentBag || value instanceof PersistentIdentifierBag || value instanceof PersistentList) {
            return List.class.getName();
        } else if (value instanceof PersistentSortedMap) {
            return TreeMap.class.getName();
        } else if (value instanceof PersistentSortedSet) {
            return TreeSet.class.getName();
        } else if (value instanceof PersistentMap) {
            return HashMap.class.getName();
        } else if (value instanceof PersistentSet) {
            return HashSet.class.getName();
        } else {
            //default is JDK collection
            return value.getClass().getName();
        }
    }

    @Override
    public String idFromValueAndType(Object value, Class<?> suggestedType) {
        return idFromValue(value);
    }

    //deserialize the json annotated JDK collection class name to JavaType
    @Override
    public JavaType typeFromId(DatabindContext ctx, String id) throws IOException {
        try {
            return ctx.getConfig().constructType(Class.forName(id));
        } catch (ClassNotFoundException e) {
            throw new UnsupportedOperationException(e);
        }
    }

    @Override
    public JsonTypeInfo.Id getMechanism() {
        return JsonTypeInfo.Id.CLASS;
    }
}