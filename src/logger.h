#pragma once

#include <boost/log/trivial.hpp>
#include <ostream>

void init_logger();

#define LOG(lvl) BOOST_LOG_TRIVIAL(lvl)

#define ENDL        std::dec << "   (" << __FILE__ << ':' << __FUNCTION__ << ':' << __LINE__ << ")"

namespace std {

	template<class charT, class traits, class CONTAINER>
	typename conditional<false, typename CONTAINER::iterator, basic_ostream<charT, traits> >::type&
		operator << (basic_ostream<charT, traits>& os, const CONTAINER& x) {
		os << "[size: " << x.size();
		if (!x.empty()) {
			os << ", data: ";
			auto itr = x.begin(), itrend = x.end();
			for (int i = 0; i < 100 && itr != itrend; ++i, ++itr)
				os << *itr << ", ";
			os << "...]";
		} else
			os << "]";
		return os;
	}

	template<class charT, class traits, class PAIR_TYPE>
	typename conditional<false, typename PAIR_TYPE::first_type, basic_ostream<charT, traits> >::type&
		operator << (basic_ostream<charT, traits>& os, const PAIR_TYPE& x) {
		os << x.first << "=>" << x.second;
		return os;
	}

	template<class charT, class traits, class T>
	basic_ostream<charT, traits>&
		operator << (basic_ostream<charT, traits>& os, const reference_wrapper<T>& x) {
		os << x.get();
		return os;
	}

	template<class charT, class traits, size_t I = 0, typename...Tp>
	typename enable_if<I == sizeof...(Tp)>::type
		printTutple(basic_ostream<charT, traits>& os, const tuple<Tp...>& t) {
	}

	template<class charT, class traits, size_t I = 0, typename...Tp>
	typename enable_if < I == 0 && I < sizeof...(Tp)>::type
		printTutple(basic_ostream<charT, traits>&os, const tuple<Tp...>&t) {
		os << get<I>(t);
		printTutple<charT, traits, I + 1, Tp...>(os, t);
	}

	template<class charT, class traits, size_t I = 0, typename...Tp>
	typename enable_if < (I > 0) && I < sizeof...(Tp)>::type
		printTutple(basic_ostream<charT, traits>& os, const tuple<Tp...>& t) {
		os << " " << get<I>(t);
		printTutple<charT, traits, I + 1, Tp...>(os, t);
	}

	template<class charT, class traits, typename...Tp>
	basic_ostream<charT, traits>&
		operator << (basic_ostream<charT, traits>& os, const tuple<Tp...>& t) {
		printTutple(os, t);
		return os;
	}

}
