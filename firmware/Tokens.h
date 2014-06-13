

#ifndef __COMMANDLINE_TOKENS__
#define __COMMANDLINE_TOKENS__

namespace CommandLine {


// Single linked list with a few quirks
// Once the user is using it current works as a built in iterator
// If the tokenizer is using it current points to the last item added


class Tokens {
	protected:
		struct Token {
			Token() : next(nullptr) {}
			Token(String st) : string(st), next(nullptr) {}
			String 	string;
			Token 	* next;
		};

		Token	* start;		// Head of linked list
		Token	* current;		// Internal iterator

		void makeReadyForUser() { current = start; }

		void add(String val) {
			if( start == nullptr )
			{
				start = new Token(val);
				current = start;
			}
			else {
				current->next = new Token(val);
				current = current->next;
			}
		}


	public:
		~Tokens() {
			erase();
		 }
		Tokens() : start(nullptr), current(nullptr) {}

		// Destructive copy constructor
		// More of a move constructor
		//Tokens( const Tokens & tk ) {
		//	this->start = tk.start;
		//	this->current = tk.current;
		//}

		class iterator {
			protected:
				Token * current;
				static String mValueReturn;
				iterator();

			public:
				iterator( Token * tk ) : current(tk) {}
				iterator( const iterator & it ) : current(it.current) {}

				iterator & next() {
					if( current )
						current = current->next;

					return *this;
				}

				iterator & operator ++() {
					return next();
				}

				// Non standard behavior of an iterator follows
				// Hiding the internals of the linked list
				String & value() {
					if( current )
						return current->string;
					//return mValueReturn;
				}


				bool isEmpty() { return (current==nullptr  ? true: false); }

				bool operator == (const iterator & it) {
					return  (current == it.current ? true : false);
				}

				bool operator != (const iterator & it) {
					return  (current == it.current ? false : true);
				}

				iterator & operator = ( const iterator & it ) {
					current = it.current;
					return *this;
				}

				// Non standard behavior
				operator String & () {
					return value();
				}

		};



		bool isEmpty() const { return (start ? false : true ); }
		int  size() const	{
			int count = 0;
			Token * tk = start;
			while( tk ) {
				++count;
				tk = tk->next;
			}
			return count;
		 }

		String getAt(int index) const {
			Token * tk = start;
			int count = 0;
			while(tk) {
				if( index == count )
					return tk->string;
				++count;
				tk = tk->next;
			}
			return String();
		}

		String operator [] (int index) const { return getAt(index); }

		// Internal iterator functions
		void reStart() { current = start; }

		// Main internal interator function
		//	Simple use:
		//		String val;
		//		while( tokens.next( val ) ) {
		//			// Do something here with val
		//			print( val );
		//		}
		bool next(String & ret)
		{
			if( current ) {
				ret = current->string;
				current = current->next;
				return true;
			}
			return false;
		}

		// Called when done with the tokens
		void erase() {
			if( !start )
				return;

			if( !start->next ){
				delete start;
				current = start = nullptr;
				return;
			}

			current = start->next;
			while( current ){
				delete start;
				start = current;
				current = current->next;
			}
			delete start;
		}

		// C++ Style iterator functions
		// 	Once erase is called or Tokens goes out of scope all iterators are
		//	invalidated. Please do not hold onto them for longer than that.
		iterator begin() { return iterator(start); }
		iterator end() { return iterator(nullptr); }

public:
		static Tokens & Tokenize(Tokens & tk, String txt, bool doQuotes = true, const char * seperators = " " );
};





}

#endif
