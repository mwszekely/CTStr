#include <CTStr/CTStr.hpp>

#include <iostream>
#include <cassert>

// This example acts as a translation system.  There's a function called 
// "translate" that takes a string and a language and returns the translated
// string. There are also a bunch of overloaded get_translation functions, 
// one for each combination of string and language. Which one is called is
// determined by overloading on the unique types of each string, and the 
// unique type of each language.

// The main advantage of this system is that there's a compile-time guarantee
// that you can't ask to translate a string that doesn't have a translation.
// If you try, you'll get an error.

enum class language
{
    english,
    japanese
};

namespace detail
{
    // Define a unique type for each language.
    // You could also do "struct english {}" but I like this better because each language gets an ID.
    using english  = std::integral_constant<int, static_cast<int>(language::english)>;
    using japanese = std::integral_constant<int, static_cast<int>(language::japanese)>;
    
    // What we're doing here is dispatching on tags, where the tags our our type'd strings.
    // All these get_translation functions are just overloaded on string "type" and language "type".
    // So if we call get_translation(string_to_type(u8"Hello!"){}, english{}),
    // we'll select the very first overloaded function.

    // Translations into English follow
    // Since we already have the strings here, we can just use to_string() to avoid duplicating them.
    std::string get_translation(string_to_type(u8"Hello!") s,                            english) { return ctstr::to_string(s); }
    std::string get_translation(string_to_type(u8"Please enter your name: \0 NORMAL") s, english) { return ctstr::to_c_string(s); }
    std::string get_translation(string_to_type(u8"Please enter your name: \0 POLITE") s, english) { return ctstr::to_c_string(s); }
    std::string get_translation(string_to_type(u8"It happened %0 times on %1/%2/%3") s,  english) { return ctstr::to_string(s); }

    // Translations into Japanese follow.
    // Note that we can abuse null characters to insert "invisible" differences between strings.
    // This can be used if some string in English has multiple translations in another language.
    // By sticking stuff after a null character, we make their types different,
    // but nothing after the null character will be read (if we use to_c_string() instead of to_string()).
    // Note how our "Please enter your name:" string is translated into two different strings here.
    std::string get_translation(string_to_type(u8"Hello!") s,                            japanese) { return u8"こんにちは！"; }
    std::string get_translation(string_to_type(u8"Please enter your name: \0 NORMAL") s, japanese) { return u8"名前を入力してください"; }
    std::string get_translation(string_to_type(u8"Please enter your name: \0 POLITE") s, japanese) { return u8"お名前をご入力して頂ければ幸いに存じます"; }
    std::string get_translation(string_to_type(u8"It happened %0 times on %1/%2/%3") s,  japanese) { return u8"%3年%1月%2日に%0回起こりますた"; }

    // Catch-all that's guaranteed to have the lowest priority of all the get_translation functions.
    // If we end up calling this function, it's because we don't have an actual translation.
    // We give it a template parameter so we can get a static_assert out of it.
    template <typename T>
    std::string get_translation(T, ...)
    {
        static_assert(!std::is_same<T, T>::value, "There is no translation for this string, or not all possible translations for it have been defined yet.");
        return{};
    }

}

// Given a string as a template parameter and a language, returns the string translated into that language.
template <typename String>
std::string translate(language lang)
{
    if (lang == language::english)
        return detail::get_translation(String{}, detail::english{});
    else if (lang == language::japanese)
        return detail::get_translation(String{}, detail::japanese{});
    else
    {
        assert(false && "Unknown language!");        // Will only get here if passed a language outside of the enum
        return detail::get_translation(String{}, detail::english{});
    }

}


int main(int argc, char* argv[])
{
    // Remember: Non-ASCII characters won't look right in the console in Windows by default.
    // On Windows either trust that the output is correct or pipe to a file.
    std::cout << translate<string_to_type(u8"Hello!")>(language::english) << std::endl;
    std::cout << translate<string_to_type(u8"Hello!")>(language::japanese) << std::endl;
    std::cout << translate<string_to_type(u8"Please enter your name: \0 NORMAL")>(language::english) << std::endl;
    std::cout << translate<string_to_type(u8"Please enter your name: \0 NORMAL")>(language::japanese) << std::endl;
    
    // This next statement would cause an error because we never defined that translation!
    // But it would be a compile-time error, not a runtime error, which, again, is way better.
    // std::cout << translate<string_to_type(u8"Goodbye!")>(language::japanese) << std::endl;
}
