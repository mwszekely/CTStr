////////////////////////////////////////////////////////////
// MIT License
// 
// Copyright (c) 2016 Matthew Szekely
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>
#include <string>
#include <utility>

////////////////////////////////////////////////////////////
/// \file
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// \defgroup CTSTR CTStr
///
/// All CTStr related functions and macros.  Yeah.  All 5 of them!
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
/// \ingroup CTSTR
/// \brief Turns a quoted \c "string" into a unique type
///
/// Creates an \c std::integer_sequence containing the characters that make up the string.
////////////////////////////////////////////////////////////
#define string_to_type(str)
#undef string_to_type



#ifndef CTSTR_MAX_STRING_SIZE
////////////////////////////////////////////////////////////
/// \ingroup CTSTR
/// \brief Defines the largest string size allowed.
///
/// Set this to whatever you want, as long as it's a power of 2 and <= 0x10000.
/// Whatever you set it to, just be aware that the larger the number is the more your compiler will hate you.
////////////////////////////////////////////////////////////
#define CTSTR_MAX_STRING_SIZE 0x100
#endif

#ifndef CTSTR_SEQUENCE_TYPE

////////////////////////////////////////////////////////////
/// \ingroup CTSTR
/// \brief Allows an alternative to \c std::integer_sequence for pre C++14 compilers
///
/// By default this macro is just \c std::integer_sequence.  If you want to use something else,
/// or your standard library doesn't provide \c std::integer_sequence, just \c #define \c CTSTR_SEQUENCE_TYPE
/// to be your own custom type.  Make sure it has the same signature as \c std::integer_sequence, though.
////////////////////////////////////////////////////////////
#define CTSTR_SEQUENCE_TYPE std::integer_sequence
#endif



////////////////////////////////////////////////////////////
/// \cond IGNORE
////////////////////////////////////////////////////////////

// Some helper macros:

// The largest "char" type.  Must be large enough to hold all possible types plus a special EOF character
#define CTSTR_EXPANDED_CHAR_TYPE std::uint64_t
// Said EOF character
#define CTSTR_INVALID_CHAR static_cast<CTSTR_EXPANDED_CHAR_TYPE>(-1)
// Determines the number of characters in a string, excluding the null terminator
#define CTSTR_STRLEN(str) ((sizeof(str) - 1) / (sizeof(*str)))



// These macros are used as helpers to build the "final" string_to_type macro


// 0001 is the most basic; for every character it either creates an std::integer_sequence containing the one character
// or an empty std::integer_sequence if the character was out of bounds.
// It looks kinda funky though.  Here's how it works:
//
// make_helper takes 3 arguments, the type of the string, the character at this point in the string, and whether the length of the string is too long for CTSTR_MAX_STRING_SIZE
// The first two are obvious.  The reason the third is there (and repeated for every character) is because there's just nowhere else to put it...
#define string_to_type_0001(str, n)								\
typename ::ctstr::detail::make_helper							\
<											\
	std::decay_t<decltype(*str)>, 							\
	::ctstr::detail::c_str_lit_at_or_eol(str, CTSTR_STRLEN(str), 0x00 + n), 	\
	CTSTR_STRLEN(str) < CTSTR_MAX_STRING_SIZE					\
>::type	

#define string_to_type_0002(str, n) ::ctstr::detail::str_cat_t<string_to_type_0001(str, n), string_to_type_0001(str, n + 0x0001)>
#define string_to_type_0004(str, n) ::ctstr::detail::str_cat_t<string_to_type_0002(str, n), string_to_type_0002(str, n + 0x0002)>
#define string_to_type_0008(str, n) ::ctstr::detail::str_cat_t<string_to_type_0004(str, n), string_to_type_0004(str, n + 0x0004)>
#define string_to_type_0010(str, n) ::ctstr::detail::str_cat_t<string_to_type_0008(str, n), string_to_type_0008(str, n + 0x0008)>
#define string_to_type_0020(str, n) ::ctstr::detail::str_cat_t<string_to_type_0010(str, n), string_to_type_0010(str, n + 0x0010)>
#define string_to_type_0040(str, n) ::ctstr::detail::str_cat_t<string_to_type_0020(str, n), string_to_type_0020(str, n + 0x0020)>
#define string_to_type_0080(str, n) ::ctstr::detail::str_cat_t<string_to_type_0040(str, n), string_to_type_0040(str, n + 0x0040)>
#define string_to_type_0100(str, n) ::ctstr::detail::str_cat_t<string_to_type_0080(str, n), string_to_type_0080(str, n + 0x0080)>
#define string_to_type_0200(str, n) ::ctstr::detail::str_cat_t<string_to_type_0100(str, n), string_to_type_0100(str, n + 0x0100)>
#define string_to_type_0400(str, n) ::ctstr::detail::str_cat_t<string_to_type_0200(str, n), string_to_type_0200(str, n + 0x0200)>
#define string_to_type_0800(str, n) ::ctstr::detail::str_cat_t<string_to_type_0400(str, n), string_to_type_0400(str, n + 0x0400)>
#define string_to_type_1000(str, n) ::ctstr::detail::str_cat_t<string_to_type_0800(str, n), string_to_type_0800(str, n + 0x0800)>
#define string_to_type_2000(str, n) ::ctstr::detail::str_cat_t<string_to_type_1000(str, n), string_to_type_1000(str, n + 0x1000)>
#define string_to_type_4000(str, n) ::ctstr::detail::str_cat_t<string_to_type_2000(str, n), string_to_type_2000(str, n + 0x2000)>
#define string_to_type_8000(str, n) ::ctstr::detail::str_cat_t<string_to_type_4000(str, n), string_to_type_4000(str, n + 0x4000)>
#define string_to_type_10000(str, n)::ctstr::detail::str_cat_t<string_to_type_8000(str, n), string_to_type_8000(str, n + 0x8000)>


#if CTSTR_MAX_STRING_SIZE > 0x10000
#error Strings longer than 0x10000 characters are not supported (and will probably destroy your compiler anyway)
#elif CTSTR_MAX_STRING_SIZE ==                 0x10000
#define string_to_type(str) string_to_type_10000(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x8000
#define string_to_type(str) string_to_type_8000(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x4000
#define string_to_type(str) string_to_type_4000(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x2000
#define string_to_type(str) string_to_type_2000(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x1000
#define string_to_type(str) string_to_type_1000(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0800
#define string_to_type(str) string_to_type_0800(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0400
#define string_to_type(str) string_to_type_0400(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0200
#define string_to_type(str) string_to_type_0200(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0100
#define string_to_type(str) string_to_type_0100(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0080
#define string_to_type(str) string_to_type_0080(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0040
#define string_to_type(str) string_to_type_0040(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0020
#define string_to_type(str) string_to_type_0020(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0010
#define string_to_type(str) string_to_type_0010(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0008
#define string_to_type(str) string_to_type_0008(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0004
#define string_to_type(str) string_to_type_0004(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0002
#define string_to_type(str) string_to_type_0002(str, 0)
#elif CTSTR_MAX_STRING_SIZE ==                 0x0001
#define string_to_type(str) string_to_type_0001(str, 0)
#else
#error CTSTR_MAX_STRING_SIZE must be a power of 2
#endif

////////////////////////////////////////////////////////////
/// \endcond
////////////////////////////////////////////////////////////

namespace ctstr
{

////////////////////////////////////////////////////////////
/// \cond INTERNAL
////////////////////////////////////////////////////////////
namespace detail
{
	template <typename Seq1, typename Seq2>
	struct str_cat;

	template <typename T, T...Seq1, T...Seq2>
	struct str_cat<std::integer_sequence<T, Seq1...>, std::integer_sequence<T, Seq2...>>
	{
		using type = std::integer_sequence<T, Seq1..., Seq2...>;
	};


	template <typename Seq1, typename Seq2>
	using str_cat_t = typename str_cat<Seq1, Seq2>::type;

	// Like \c std::strlen but constexpr
	template <typename CharType>
	constexpr std::size_t cxpr_strlen(const CharType* str)
	{
		return *str? 1 + cxpr_strlen(str + 1) : 0;
	}
	
	// Creates an std::integer_sequence containing Value
	// Also used to check the string size, because we're really limited in where we can do that.
	template <typename T, CTSTR_EXPANDED_CHAR_TYPE Value, bool Valid>
	struct make_helper
	{
		static_assert(Valid, "This string is too long; it won't fit in MAX_STRING_SIZE.  Increase MAX_STRING_SIZE or shorten this string.");
		using type = std::integer_sequence<T, Value>;
	};

	// Same as above, but specialized for 0 to make std::integer_sequence<T, 0> go to std::integer_sequence<T>
	// This is the reason Value above is forced into being a char32_t; we can't specialize it otherwise.
	template <typename T, bool Valid>
	struct make_helper<T, CTSTR_INVALID_CHAR, Valid>
	{
		static_assert(Valid, "This string is too long; it won't fit in MAX_STRING_SIZE.  Increase MAX_STRING_SIZE or shorten this string.");
		using type = std::integer_sequence<T>;
	};
	
	template <typename CharType>
	constexpr CTSTR_EXPANDED_CHAR_TYPE c_str_lit_at_or_eol(const CharType* str, std::size_t strLength, std::size_t index)
	{
		return index >= strLength? CTSTR_INVALID_CHAR : static_cast<CTSTR_EXPANDED_CHAR_TYPE>(str[index]);
	}



	//template <typename SeqType>
	//struct to_string_helper;
	//
	//template <typename CharType, CharType...Chars>
	//struct to_string_helper<std::integer_sequence<CharType, Chars...>>
	//{
	//	static auto get()
	//	{
	//		return std::basic_string<CharType, std::char_traits<CharType>>({Chars...});
	//	}
	//};

	template <typename T, T...t>
	auto to_string_helper(std::integer_sequence<T, t...>)
	{
		return std::basic_string<T>({ t... });
	}


	template <typename T, T...t>
	auto to_c_string_helper(std::integer_sequence<T, t...>)
	{
		const static T array[] = { t..., '\0' };
		return array;
	}
	}

template <typename T>
struct sequence_type;
template <typename T, T...t>
struct sequence_type<std::integer_sequence<T, t...>> { using type = T; };

////////////////////////////////////////////////////////////
/// \endcond
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// \ingroup CTSTR
/// \brief Turns a string returned from \c string_to_type into an \c std::string
///
/// You can either use <tt>to_string<string_to_type("foo")>()</tt> or <tt>to_string(string_to_type("foo"){})</tt>.
/// Pick whichever you find to be neater.
///
/// The only other thing of note is that it's actually an \c std::basic_string with
/// the char type the type used in the \c std::integer_sequence you created.
/// Besides that, it's literally just creating a string that contains your characters.
///
/// \see to_c_string()
///
/// \param sequence Optional.  Used solely to avoid explicity typing the template argument.
///
/// \return An \c std::basic_string containing your characters
////////////////////////////////////////////////////////////
template <typename SeqType>
auto to_string(SeqType sequence = SeqType{})
{
	return detail::to_string_helper(sequence);
}


////////////////////////////////////////////////////////////
/// \ingroup CTSTR
/// \brief Turns a string returned from \c string_to_type into a C-style \c "string"
///
/// This is just like \c to_string() except the returned type is a pointer to a sequence of characters.
/// You do not need to \c delete or \c free this memory.  It exists as a static array.
///
/// Under most circumstances you'll probably just want to stick to \c to_string().  But if you have
/// a function that just takes a <tt>const char*</tt> and want to save the memory allocation and
/// calling <tt>str.c_str()</tt> and whatnot, this works too.
///
/// \see to_string()
///
/// \param sequence Optional.  Used solely to avoid explicity typing the template argument.
///
/// \return An <tt>const char*</tt> to your characters (or <tt>const char32_t*</tt> or whatever)
////////////////////////////////////////////////////////////
template <typename SeqType>
auto to_c_string(SeqType sequence = SeqType{})
{
	return detail::to_c_string_helper(sequence);
}


}




////////////////////////////////////////////////////////////
/// \mainpage CTStr: Compile-Time Strings
///
/// <a href="https://github.com/mwszekely/CTStr/>Link to GitHub</a>
///
/// \section SuperBasic 5-Second Rundown
/// If someone redirected you here and you want the 5-second rundown, this is what this library lets you do:
/// \code
/// static_assert(std::is_same<
///                            string_to_type("foo"), 
///                            std::integer_sequence<char, 'f', 'o', 'o'>
///                           >::value);
/// \endcode
///
/// In other words, this lets you turn a quoted \c "string" into an \c std::integer_sequence which can be used in a template argument.
///
/// This is a tiny library.  Literally just 1 important function/macro, 2 helper functions, and 2 settings macros all in one header file.
///
/// \section IntroSec Introduction
/// C++ doesn't allow (inline) strings as template arguments.  For example, it's impossible to do this:
/// \code
///
/// constexpr bool check_string(const char* str)
/// {
///     // Do something with str here--make sure it's less than a certain length maybe.
/// }
///
/// template <const char* str>
/// void frob()
/// {
///     static_assert(check_string(str), "Can't use this string!");
///     // Do something with str
/// }
///
/// int main()
/// {
///     frob<"foo">();      // Won't work
///
///     extern char str[];
///     frob<str>();        // Works, but is super ugly and requires the definition of str be somewhere else
/// }
///
/// char string[] = "foo";
/// \endcode
/// \c static_assert is awesome, and it's a shame that it's impossible really to make it work cleanly with string literals (among plenty of other things you could do with compile-time string literals).
///
/// This tiny library aims to solve that.
///
/// \section How How?
///
/// C++ doesn't allow C-style strings as template arguments.  But other things are fair game.  Specifically the \c integer_sequence class.
/// \c integer_sequence is a new class made possible with C++11 and made available in C++14.  It's literally an empty class that just encodes a series of numbers in its type.
/// <tt>template <typename T, T...Values> class integer_sequence{};</tt> is literally the whole definition of the class.
///
/// So what we're going to do is take a C-style string and put its characters into an \c integer_sequence.  The end result will look like <tt>integer_sequence<char, 'f', 'o', 'o'></tt>.
/// But that is, as I'm sure anyone would agree, unbelievably annoying to type.  4-5 keystrokes per character.  Awful.  So we want a way to automate the process.
///
/// \section StringToType Enter string_to_type
///
/// To solve our problem we'll use a handy macro that will take a string and spit out a type.  Specifically that \c integer_sequence type I mentioned earlier.
/// And that's it!  That's all we need.  The macro in question is \c string_to_type.
///
/// Usage is very simple.
/// \code
/// using SomeStringType = string_to_type("foo");            // SomeStringType is the same as integer_sequence<char, 'f', 'o', 'o'>
///
/// std::cout << ctstr::to_string<SomeStringType>();         // Because SomeStringType is in fact a type, you'll pass it as a template argument.
/// std::cout << ctstr::to_string(SomeStringType{});         // ...But for convenience's sake this is also acceptable.
///
/// std::cout << ctstr::to_string<string_to_type("foo")>();  // This is also fine. string_to_type can be used inline anywhere a normal template argument is expected.
/// std::cout << ctstr::to_string(string_to_type("foo"){});  // And of course we have the convenient method as well.
///
///
///
/// using U32StringType = string_to_type(U"F͆̆̊́͊ͯͯ̆̐̀̊̔̆̐ͬͮ̅öò");            // And of course this works too.  
///                                                          // The type is std::integer_sequence<char32_t, ...>.  
///                                                          // But watch out for combining characters!
///                                                          // This sequence is 20 integers long!
///                                                          // In fact, it could be longer depending on what normalization form the string's in.
///                                                          // In general, I'd stay away from using Unicode strings for this reason.
///                                                          // You could get some unexpected (but totally legal) results.
/// 
/// \endcode
///
/// So with the basics out of the way, let's say we wanted to rewrite our code above.  We'd do this:
/// \code
/// template <typename String>
/// constexpr bool check_string()
/// {
///     // Do something with String here--make sure it's less than a certain length maybe.
///     // Remember that String is just an integer_sequence.  
///     // You'll want to break out your functional programming techniques here.
/// }
///
/// template <typename String>
/// void frob()
/// {
///     static_assert(check_string<String>(), "Can't use this string!");
///     // Do something with String
/// }
///
/// int main()
/// {
///     frob<string_to_type("foo")>();      // THE IMPORTANT PART! LOOK HERE!
/// }
/// \endcode
///
/// The changes, in order:
/// 1. \c check_string changed.  It now takes a template parameter instead of a normal argument because we're going to be passing it types instead of objects.
/// 2. \c frob now takes a type parameter.  We call it \c String, and when called correctly it will hold an \c std::integer_sequence of some kind.
/// 3. We use \c string_to_type in our call to \c frob.
///
/// But really the important part is the call to the \c string_to_type macro.  As a reminder, this just takes your string, \c "foo", and "explodes" it into individual characters
/// which are all put into an \c std::integer_sequence.  That integer sequence is the result.  So when you call <tt>frob<string_to_type("foo")></tt>, you're
/// really calling <tt>frob<std::integer_sequence<char, 'f', 'o', 'o'>></tt>.  What you choose to do in frob with this \c integer_sequence is up to you.
///
/// \section Details The gory details
///
/// So I mentioned macros before and this is where things get...hairy.  
/// Thankfully C++11 introduced the notion of \c constexpr to make this whole library possible.
/// Less thankfully constexpr is extremely limited in what it can do.  In particular it cannot take constexpr arguments.  
/// What this means, more or less, is that the only thing we can use to build our type is the return values of functions.
/// We cannot build part of our sequence in the function itself, because that requires a constant expression, and \c constexpr functions don't have any guarantee
/// on the "constexpr-ness" of their arguments.  The usual way to accept a "constexpr argument" is to take it as a template argument.  And we'd totally do that!  
/// ...if strings could be taken as template arguments.
///
/// So we can't have a function that takes a string and returns an \c std::integer_sequence or any part of one is what I'm saying.  It's just impossible.  Instead we need a function that takes a single char and returns a part of an \c std::integer_sequence.  A lot of these functions.
///
/// So what we do is create a macro.  This macro basically looks like
/// \code
/// #define macro(str) std::integer_sequence<char, get_char(str, 0), get_char(str, 1), get_char(str, 2), ...>
/// \endcode
/// Though it's more sophisticated than that.  For example, you can still do <tt>string_to_type(U"foo")</tt> and you'll get an <tt>std::integer_sequence<char32_t, ...></tt>.
/// There are also checks to make sure the string isn't too long.
///
/// "Too long?" So there's a maximum length on these strings.  
/// Since macros are way, way more limited than the actual C++ language itself what we basically do is repeat the "get_char(str, X)" pattern a gazillion times.
/// The meaning of "gazillion" is, by default, 256, and can be changed by defining CTSTR_MAX_STRING_SIZE to be something else (just make sure it's a power of 2).
///
/// Anyway, if you try to make a string that's too long (so by default a string that's more than 256 characters), you'll get a \c static_assert failure, which is nice.
///
/// \section Example An actual example
///
/// This example acts as a translation system.  There's a function called \c translate that takes a string and a language and returns the translated string.
/// There are also a bunch of overloaded \c get_translation functions, one for each combination of string and language.
/// Which one is called is determined by overloading on the unique types of each string, and the unique type of each language.
///
/// The main advantage of this system is that there's a compile-time guarantee that you can't ask to translate a string that doesn't have a translation.
/// If you try, you'll get an error.
///
/// \code
/// #include <CTStr/CTStr.hpp>
///
/// enum class language
/// {
///     english,
///     japanese
/// };
/// 
/// namespace detail
/// {
///     // Define a unique type for each language.
///     // You could also do "struct english {}" but I like this better because each language gets an ID.
///     using english  = std::integral_constant<int, static_cast<int>(language::english)>;
///     using japanese = std::integral_constant<int, static_cast<int>(language::japanese)>;
///     
///     // What we're doing here is dispatching on tags, where the tags our our type'd strings.
///     // All these get_translation functions are just overloaded on string "type" and language "type".
///     // So if we call get_translation(string_to_type(u8"Hello!"){}, english{}),
///     // we'll select the very first overloaded function.
///
///     // Translations into English follow
///     // Since we already have the strings here, we can just use to_string() to avoid duplicating them.
///     std::string get_translation(string_to_type(u8"Hello!") s,                            english) { return ctstr::to_string(s); }
///     std::string get_translation(string_to_type(u8"Please enter your name: \0 NORMAL") s, english) { return ctstr::to_c_string(s); }
///     std::string get_translation(string_to_type(u8"Please enter your name: \0 POLITE") s, english) { return ctstr::to_c_string(s); }
///     std::string get_translation(string_to_type(u8"It happened %0 times on %1/%2/%3") s,  english) { return ctstr::to_string(s); }
/// 
///     // Translations into Japanese follow.
///     // Note that we can abuse null characters to insert "invisible" differences between strings.
///     // This can be used if some string in English has multiple translations in another language.
///     // By sticking stuff after a null character, we make their types different,
///     // but nothing after the null character will be read (if we use to_c_string() instead of to_string()).
///     // Note how our "Please enter your name:" string is translated into two different strings here.
///     std::string get_translation(string_to_type(u8"Hello!") s,                            japanese) { return u8"こんにちは！"; }
///     std::string get_translation(string_to_type(u8"Please enter your name: \0 NORMAL") s, japanese) { return u8"名前を入力してください"; }
///     std::string get_translation(string_to_type(u8"Please enter your name: \0 POLITE") s, japanese) { return u8"お名前をご入力してくださいませ"; }
///     std::string get_translation(string_to_type(u8"It happened %0 times on %1/%2/%3") s,  japanese) { return u8"%3年%1月%2日に%0回起こりますた"; }
///
///     // Catch-all that's guaranteed to have the lowest priority of all the get_translation functions.
///     // If we end up calling this function, it's because we don't have an actual translation.
///     // We give it a template parameter so we can get a static_assert out of it.
///     template <typename T>
///     std::string get_translation(T, ...)
///     {
///         static_assert(!std::is_same<T, T>::value, "There is no translation for this string, or not all possible translations for it have been defined yet.");
///     }
/// 
/// }
/// 
/// template <typename String>
/// std::string translate(language lang)
/// {
///     if (lang == language::english)
///         return detail::get_translation(String{}, detail::english{});
///     else if (lang == language::japanese)
///         return detail::get_translation(String{}, detail::japanese{});
///     else
///         assert(false && "Unknown language!");
///
///     return detail::get_translation(String{}, detail::english{});
/// }
///
///
/// int main(int argc, char* argv[])
/// {
///     // Remember: Non-ASCII characters won't look right in the console in Windows by default.
///     std::cout << translate<string_to_type(u8"Hello!")>(language::english) << std::endl;
///     std::cout << translate<string_to_type(u8"Hello!")>(language::japanese) << std::endl;
///     std::cout << translate<string_to_type(u8"Please enter your name: \0 NORMAL")>(language::english) << std::endl;
///     std::cout << translate<string_to_type(u8"Please enter your name: \0 NORMAL")>(language::japanese) << std::endl;
///     
///     // This would cause an error because we never defined that translation!
///     // But it would be a compile-time error, not a runtime error, which, again, is way better.
///     // std::cout << translate<string_to_type(u8"Goodbye!")>(language::japanese) << std::endl;
/// }
/// \endcode
///
/// \section Summary In Summary
///
///  - Use <tt>string_to_type("string")</tt> to create an \c std::integer_sequence that contains the same characters as \c "string".
///  - All string types are supported, so <tt>string_to_type(U"string")</tt> works too.
///  - This library only creates the strings.  If you want things like string splitting, finding, etc. you'll want a library that operates on <tt>std::integer_sequence</tt>s.
/// This is mostly a matter of practicality; there's no point in reinventing the wheel when other solutions exist already.
///  - If your compiler/library doesn't support C++14 and so doesn't have \c std::integer_sequence, make your own and \c #define \c CTSTR_SEQUENCE_TYPE to be your custom type.
///  - *Be careful if you're using Unicode characters in your strings!* Seriously.  
/// If you don't know what Unicode Normalization Forms are, all you need to know that it's *not* safe to just compare two Unicode string together without some additional preprocessing.
/// Save yourself the trouble of wondering why <tt>static_assert(std::is_same<string_to_type(U"á"), string_to_type(U"á")>::value)</tt> is failing and just stay away.
/// (Protip: The first is <tt>std::integer_sequence<char32_t, 97, 769></tt> and the second is <tt>std::integer_sequence<char32_t, 225></tt>. Both are considered canonically equivilant but \c is_same doesn't know that.)
////////////////////////////////////////////////////////////


