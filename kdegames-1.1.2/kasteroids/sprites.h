
#ifndef __SPRITES_H__
#define __SPRITES_H__


#include <QwSpriteField.h>

#define LARGE_ROCK	1024
#define MEDIUM_ROCK	1025
#define SMALL_ROCK	1026

#define RTTI_MISSILE	1030
#define MAX_MISSILE_AGE	60

#define RTTI_BIT	1040

class KMissile : public QwRealMobileSprite
{
public:
    KMissile (QwSpritePixmapSequence *s) : QwRealMobileSprite( s )
	{ myAge = 0; }
    virtual ~KMissile() {}

    virtual int rtti() const
	{   return RTTI_MISSILE; }

    void growOlder() { myAge++; }
    bool expired() { return myAge > MAX_MISSILE_AGE; }

private:
    int myAge;
};

class KBit : public QwRealMobileSprite
{
public:
    KBit( QwSpritePixmapSequence *s ) : QwRealMobileSprite( s )
	{  death = 7; }
    virtual ~KBit() {}

    virtual int rtti() const
	{  return RTTI_BIT; }

    void setDeath( int d ) { death = d; }
    void growOlder() { death--; }
    bool expired() { return death <= 0; }

private:
    int death;
};

class KLargeRock : public QwRealMobileSprite
{
public:
    KLargeRock (QwSpritePixmapSequence *s) : QwRealMobileSprite( s )
	{}
    virtual ~KLargeRock() {}

    virtual int rtti() const
	{   return LARGE_ROCK; }
};

class KMediumRock : public QwRealMobileSprite
{
public:
    KMediumRock (QwSpritePixmapSequence *s) : QwRealMobileSprite( s )
	{}
    virtual ~KMediumRock() {}

    virtual int rtti() const
	{   return MEDIUM_ROCK; }
};

class KSmallRock : public QwRealMobileSprite
{
public:
    KSmallRock (QwSpritePixmapSequence *s) : QwRealMobileSprite( s )
	{}
    virtual ~KSmallRock() {}

    virtual int rtti() const
	{   return SMALL_ROCK; }
};

#endif

