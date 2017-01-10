
LDFLAGS = $(QT_LDFLAGS) $(X_LDFLAGS)

# the difference between the first and the last, is the major number
# the last is the minor number
# the number in the middle is the minor-minor number
libQwSpriteField_la_LDFLAGS = -version-info 6:0:5  -lXext  $(LIB_QT)

INCLUDES = $(QT_INCLUDES)
lib_LTLIBRARIES = libQwSpriteField.la

libQwSpriteField_la_SOURCES = QwSpriteField.cpp QwSpriteField-double.cpp \
	QwSpriteField-int.cpp QwSpriteField-mobdouble.cpp \
	QwSpriteField-mobint.cpp QwCluster.cpp moc_QwSpriteField.cpp

EXTRA_DIST = QwPublicList.cpp QwSpriteField-template.cpp

include_HEADERS = Pix.h QwCluster.h QwPublicList.h QwSpriteField.h

BUILTSOURCES = moc_QwSpriteField.cpp moc_QwSpriteField.cpp
CLEANFILES = $(BUILTSOURCES)

check_PROGRAMS = example
example_SOURCES = example.cpp moc_example.cpp
example_LDADD = libQwSpriteField.la   $(LIB_QT) 

moc_QwSpriteField.cpp: QwSpriteField.h
	$(MOC) $(srcdir)/QwSpriteField.h -o moc_QwSpriteField.cpp

moc_example.cpp: example.h
	$(MOC) $(srcdir)/example.h -o moc_example.cpp



