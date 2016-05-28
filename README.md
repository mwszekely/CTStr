#CTStr: Compile-Time Strings

##5-Second Rundown
If you just got here and you want the super short version, this is what this library lets you do:
```C++
static_assert(std::is_same<
                           string_to_type("foo"), 
                           std::integer_sequence<char, 'f', 'o', 'o'>
                          >::value);
```

In other words, this lets you turn a quoted `"string"` into an `std::integer_sequence` which can be used in a template argument.

This is a tiny library.  Literally just 1 important function/macro, 2 helper functions, and 2 settings macros all in one header file.

##Introduction
C++ doesn't allow (inline) strings as template arguments.  For example, it's impossible to do this:
```C++
constexpr bool check_string(const char* str)
{
    // Do something with str here--make sure it's less than a certain length maybe.
}

template <const char* str>
void frob()
{
    static_assert(check_string(str), "Can't use this string!");
    // Do something with str
}

int main()
{
    frob<"foo">();      // Won't work

    extern char str[];
    frob<str>();        // Works, but is super ugly and requires the definition of str be somewhere else
}

char string[] = "foo";
```
`static_assert` is awesome, and it's a shame that it's impossible really to make it work cleanly with string literals (among plenty of other things you could do with compile-time string literals).

This tiny library aims to solve that.

##How?

C++ doesn't allow C-style strings as template arguments.  But other things are fair game.  Specifically the `integer_sequence` class. `integer_sequence` is a new class made possible with C++11 and made available in C++14.  It's literally an empty class that just encodes a series of numbers in its type. `template <typename T, T...Values> class integer_sequence{};` is literally the whole definition of the class.

So what we're going to do is take a C-style string and put its characters into an `integer_sequence`.  The end result will look like `integer_sequence<char, 'f', 'o', 'o'>`. But that is, as I'm sure anyone would agree, unbelievably annoying to type.  4-5 keystrokes per character.  Awful.  So we want a way to automate the process.

##Enter `string_to_type`

To solve our problem we'll use a handy macro that will take a string and spit out a type.  Specifically that `integer_sequence` type I mentioned earlier. And that's it!  That's all we need.  The macro in question is `string_to_type`.

Usage is very simple:
```C++
using SomeStringType = string_to_type("foo");            // SomeStringType is the same as integer_sequence<char, 'f', 'o', 'o'>

std::cout << ctstr::to_string<SomeStringType>();         // Because SomeStringType is in fact a type, you'll pass it as a template argument.
std::cout << ctstr::to_string(SomeStringType{});         // ...But for convenience's sake this is also acceptable.

std::cout << ctstr::to_string<string_to_type("foo")>();  // This is also fine. string_to_type can be used inline anywhere a normal template argument is expected.
std::cout << ctstr::to_string(string_to_type("foo"){});  // And of course we have the convenient method as well.



using U32StringType = string_to_type(U"F͆̆̊́͊ͯͯ̆̐̀̊̔̆̐ͬͮ̅öò");            // And of course this works too.  
                                                         // The type is std::integer_sequence<char32_t, ...>.  
                                                         // But watch out for combining characters!
                                                         // This sequence is 20 integers long!
                                                         // In fact, it could be longer depending on what normalization form the string's in.
                                                         // In general, I'd stay away from using Unicode strings for this reason.
                                                         // You could get some unexpected (but totally legal) results.
```

So with the basics out of the way, let's say we wanted to rewrite our code above.  We'd do this:
```C++
template <typename String>
constexpr bool check_string()
{
    // Do something with String here--make sure it's less than a certain length maybe.
    // Remember that String is just an integer_sequence.  
    // You'll want to break out your functional programming techniques here.
}

template <typename String>
void frob()
{
    static_assert(check_string<String>(), "Can't use this string!");
    // Do something with String
}

int main()
{
    frob<string_to_type("foo")>();      // THE IMPORTANT PART! LOOK HERE!
}
```

The changes, in order:
 1. `check_string` changed.  It now takes a template parameter instead of a normal argument because we're going to be passing it types instead of objects.
 2. `frob` now takes a type parameter.  We call it `String`, and when called correctly it will hold an `std::integer_sequence` of some kind.
 3. We use `string_to_type` in our call to `frob`.

But really the important part is the call to the `string_to_type` macro.  As a reminder, this just takes your string, `"foo"`, and "explodes" it into individual characters which are all put into an `std::integer_sequence`.  That integer sequence is the result.  So when you call `frob<string_to_type("foo")>`, you're really calling `frob<std::integer_sequence<char, 'f', 'o', 'o'>>`.  What you choose to do in `frob` with this `integer_sequence` is up to you.

##The gory details

So I mentioned macros before and this is where things get...hairy.  Thankfully C++11 introduced the notion of `constexpr` to make this whole library possible. Less thankfully constexpr is extremely limited in what it can do.  In particular it cannot take constexpr arguments. What this means, more or less, is that the only thing we can use to build our type is the return values of functions. We cannot build part of our sequence in the function itself, because that requires a constant expression, and `constexpr` functions don't have any guarantee on the "constexpr-ness" of their arguments. The usual way to accept a "constexpr argument" is to take it as a template argument.  And we'd totally do that! ...if strings could be taken as template arguments.

So we can't have a function that takes a string and returns an `std::integer_sequence` or any part of one is what I'm saying.  It's just impossible.  Instead we need a function that takes a single char and returns a part of an `std::integer_sequence`.  A lot of these functions.

So what we do is create a macro.  This macro basically looks like
```C++
#define macro(str) std::integer_sequence<char, get_char(str, 0), get_char(str, 1), get_char(str, 2), ...>
```
Though it's more sophisticated than that.  For example, you can still do `string_to_type(U"foo")` and you'll get an `std::integer_sequence<char32_t, ...>`. There are also checks to make sure the string isn't too long.

"Too long?" So there's a maximum length on these strings.  
Since macros are way, way more limited than the actual C++ language itself what we basically do is repeat the `"get_char(str, X)"` pattern a gazillion times.
The meaning of "gazillion" is, by default, 256, and can be changed by defining `CTSTR_MAX_STRING_SIZE` to be something else (just make sure it's a power of 2).

Anyway, if you try to make a string that's too long (so by default a string that's more than 256 characters), you'll get a `static_assert` failure, which is nice.

##An actual example

This example acts as a (very simple) translation system.  There's a function called `translate` that takes a string and a language and returns the translated string. There are also a bunch of overloaded `get_translation` functions, one for each combination of string and language. Which one is called is determined by overloading on the unique types of each string, and the unique type of each language.

The main advantage of this system is that there's a compile-time guarantee that you can't ask to translate a string that doesn't have a translation. If you try, you'll get an error.

```C++
#include <CTStr/CTStr.hpp>

#include <iostream>
#include <cassert>

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
    
    // This would cause an error because we never defined that translation!
    // But it would be a compile-time error, not a runtime error, which, again, is way better.
    // std::cout << translate<string_to_type(u8"Goodbye!")>(language::japanese) << std::endl;
}
```

##In Summary

 - Use `string_to_type("string")` to create an `std::integer_sequence` that contains the same characters as `"string"`.
 - All string types are supported, so `string_to_type(U"string")` works too.
 - This library only creates the strings.  If you want things like string splitting, finding, etc. you'll want a library that operates on `std::integer_sequence`s. This is mostly a matter of practicality; there's no point in reinventing the wheel when other solutions exist already.
 - If your compiler/library doesn't support C++14 and so doesn't have `std::integer_sequence`, make your own and `#define` `CTSTR_SEQUENCE_TYPE` to be your custom type.
 - *Be careful if you're using Unicode characters in your strings!* Seriously. If you know what Unicode Normalization Forms are, then you already know why.  If you don't, then just save yourself the trouble of wondering why `static_assert(std::is_same<string_to_type(U"á"), string_to_type(U"á")>::value)` is failing and just stay away. (Protip: The first is `std::integer_sequence<char32_t, 97, 769>` and the second is `std::integer_sequence<char32_t, 225>`. Both are considered canonically equivilant but `std::is_same` has no way of knowing that.)
 - The default maximum size of a compile-time string is 0x100.  Trying to create a string longer than that will result in a compile-time error.  If you need longer strings, `#define` `CTSTR_MAX_STRING_SIZE` to be any power of 2 between 1 and 0x10000 (though keep in mind compiler performance when increasing the limit).
