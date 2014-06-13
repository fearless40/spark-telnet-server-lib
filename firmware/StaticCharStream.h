
#ifndef __TELNET_STATICCHARSTREAM__
#define __TELNET_STATICCHARSTREAM__

namespace Telnet {

template <int StreamSize>
class StaticCharStream
{
private:
	char  buffer[StreamSize];
	unsigned int mEndStream;


public:
	StaticCharStream<StreamSize> & addChar( char c ) {
		if( mEndStream >= StreamSize )
			return *this;

		buffer[mEndStream++] = c;
			return *this;
	}


	StaticCharStream<StreamSize> & operator << (char c) {
		return addChar(c);
	}

	char getAt(unsigned int index) const {
		if( index <= mEndStream && index < StreamSize && index >= 0  )
			return buffer[index];
		else
			return 0;
	}

	char operator [] ( unsigned int index ) const { return getAt(index); }

	void reset() { mEndStream = 0; }

	char last()  const { return (mEndStream > 0) ? buffer[mEndStream-1] : 0; }

	char right(int relative) const {
		return ( (mEndStream - relative - 1) < 0 ) ? 0 : buffer[mEndStream - relative - 1];
	}

	char left( int relative) const {
		return getAt(relative);
	}

	bool full() {
		return mEndStream >= StreamSize ? true : false;
	}

};

};

#endif
