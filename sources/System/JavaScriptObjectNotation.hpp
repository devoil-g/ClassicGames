#pragma once

/**
 * @file JavaScriptObjectNotation.hpp
 * @brief C++ implementation of the JSON (JavaScript Object Notation) data format.
 */

#include <filesystem>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

namespace Game
{
  /**
   * @brief Namespace providing a C++ implementation of the JSON (JavaScript Object Notation) data format.
   *
   * Supports parsing, building and serializing JSON structures including objects, arrays,
   * numbers, strings, booleans and null values. All string data uses wide characters (wchar_t).
   */
  namespace JavaScriptObjectNotation
  {
    // Forward declaration of JSON types
    class Object;
    class Array;
    class Number;
    class String;
    class Boolean;
    class Null;

    /**
     * @brief Enumerates the possible types of a JSON element.
     */
    enum class Type
    {
      Object,  // A collection of name–value pairs
      Array,   // An ordered list of zero or more elements
      Number,  // A signed decimal number
      String,  // A sequence of zero or more characters
      Boolean, // Either of the values true or false
      Null     // An empty value
    };

    /**
     * @brief Abstract base interface for all JSON element types.
     *
     * Provides a common polymorphic interface to access, query and serialize any JSON value.
     * Typed accessors throw @ref TypeError when the element is not of the expected type.
     */
    class Element
    {
    public:
      Element() = default;
      Element(const Element&) = default;
      Element(Element&&) = default;
      virtual ~Element() = 0;

      Element& operator=(const Element&) = default;
      Element& operator=(Element&&) = default;

      /**
       * @brief Returns the type of this JSON element.
       * @return The @ref Type value corresponding to this element's concrete type.
       */
      virtual Type  type() const = 0;

      /**
       * @brief Checks whether this element is of null type.
       * @return true if the element is @ref Null, false otherwise.
       */
      virtual bool null() const;

      /**
       * @brief Returns a mutable reference to the underlying @ref Object.
       * @return Reference to this element as an @ref Object.
       * @throws TypeError if this element is not an @ref Object.
       */
      virtual Object& object();

      /**
       * @brief Returns a mutable reference to the underlying @ref Array.
       * @return Reference to this element as an @ref Array.
       * @throws TypeError if this element is not an @ref Array.
       */
      virtual Array& array();

      /**
       * @brief Returns a mutable reference to the underlying numeric value.
       * @return Reference to the double value held by this element.
       * @throws TypeError if this element is not a @ref Number.
       */
      virtual double& number();

      /**
       * @brief Returns a mutable reference to the underlying string value.
       * @return Reference to the wide string value held by this element.
       * @throws TypeError if this element is not a @ref String.
       */
      virtual std::wstring& string();

      /**
       * @brief Returns a mutable reference to the underlying boolean value.
       * @return Reference to the bool value held by this element.
       * @throws TypeError if this element is not a @ref Boolean.
       */
      virtual bool& boolean();

      /**
       * @brief Returns a const reference to the underlying @ref Object.
       * @return Const reference to this element as an @ref Object.
       * @throws TypeError if this element is not an @ref Object.
       */
      virtual const Object& object() const;

      /**
       * @brief Returns a const reference to the underlying @ref Array.
       * @return Const reference to this element as an @ref Array.
       * @throws TypeError if this element is not an @ref Array.
       */
      virtual const Array& array() const;

      /**
       * @brief Returns the underlying numeric value by copy.
       * @return The double value held by this element.
       * @throws TypeError if this element is not a @ref Number.
       */
      virtual double              number() const;

      /**
       * @brief Returns a const reference to the underlying string value.
       * @return Const reference to the wide string value held by this element.
       * @throws TypeError if this element is not a @ref String.
       */
      virtual const std::wstring& string() const;

      /**
       * @brief Returns the underlying boolean value by copy.
       * @return The bool value held by this element.
       * @throws TypeError if this element is not a @ref Boolean.
       */
      virtual bool                boolean() const;

      /**
       * @brief Serializes this JSON element to its string representation.
       * @return A wide string containing the JSON-encoded value.
       */
      virtual std::wstring stringify() const = 0;
    };

    /**
     * @brief Represents a JSON object: an unordered collection of key-value pairs.
     *
     * Keys are wide strings and values are any JSON @ref Element. Supports construction
     * from a file path or a JSON-encoded wide string, as well as full CRUD operations
     * on its entries.
     */
    class Object : public Element
    {
    private:
      std::unordered_map<std::wstring, std::unique_ptr<Element>> _map; ///< Internal storage of key-value pairs.

      /**
       * @brief Parses and loads the object's content from a JSON-encoded wide string.
       * @param text The JSON wide string to parse.
       * @throws ParsingError if the text is not valid JSON.
       */
      void load(const std::wstring& text);

      /**
       * @brief Advances the iterator past any whitespace characters.
       * @param text      The full source JSON wide string.
       * @param iterator  Iterator into @p text, advanced in-place.
       */
      static void                     loadWhitespaces(const std::wstring& text, std::wstring::const_iterator& iterator);

      /**
       * @brief Parses and returns the next JSON element starting at the iterator position.
       * @param text      The full source JSON wide string.
       * @param iterator  Iterator into @p text, advanced past the parsed element.
       * @return A heap-allocated @ref Element representing the parsed value.
       * @throws ParsingError if the content at the iterator is not a valid JSON value.
       */
      static std::unique_ptr<Element> loadElement(const std::wstring& text, std::wstring::const_iterator& iterator);

      /**
       * @brief Parses a JSON object literal into the provided @ref Object instance.
       * @param text      The full source JSON wide string.
       * @param iterator  Iterator into @p text positioned at the opening '{', advanced past the closing '}'.
       * @param object    The target @ref Object to populate.
       * @throws ParsingError if the content is not a valid JSON object.
       */
      static void loadObject(const std::wstring& text, std::wstring::const_iterator& iterator, Object& object);

      /**
       * @brief Parses a JSON array literal into the provided @ref Array instance.
       * @param text      The full source JSON wide string.
       * @param iterator  Iterator into @p text positioned at the opening '[', advanced past the closing ']'.
       * @param array     The target @ref Array to populate.
       * @throws ParsingError if the content is not a valid JSON array.
       */
      static void loadArray(const std::wstring& text, std::wstring::const_iterator& iterator, Array& array);

      /**
       * @brief Parses a JSON number literal into the provided @ref Number instance.
       * @param text      The full source JSON wide string.
       * @param iterator  Iterator into @p text positioned at the start of the number, advanced past it.
       * @param number    The target @ref Number to populate.
       * @throws ParsingError if the content is not a valid JSON number.
       */
      static void loadNumber(const std::wstring& text, std::wstring::const_iterator& iterator, Number& number);

      /**
       * @brief Parses a JSON string literal into the provided @ref String instance.
       * @param text      The full source JSON wide string.
       * @param iterator  Iterator into @p text positioned at the opening '"', advanced past the closing '"'.
       * @param string    The target @ref String to populate.
       * @throws ParsingError if the content is not a valid JSON string.
       */
      static void loadString(const std::wstring& text, std::wstring::const_iterator& iterator, String& string);

      /**
       * @brief Parses a JSON boolean literal into the provided @ref Boolean instance.
       * @param text      The full source JSON wide string.
       * @param iterator  Iterator into @p text positioned at 't' or 'f', advanced past the literal.
       * @param boolean   The target @ref Boolean to populate.
       * @throws ParsingError if the content is not a valid JSON boolean.
       */
      static void loadBoolean(const std::wstring& text, std::wstring::const_iterator& iterator, Boolean& boolean);

      /**
       * @brief Parses a JSON null literal into the provided @ref Null instance.
       * @param text      The full source JSON wide string.
       * @param iterator  Iterator into @p text positioned at 'n', advanced past the literal.
       * @param null      The target @ref Null to populate.
       * @throws ParsingError if the content is not the literal "null".
       */
      static void loadNull(const std::wstring& text, std::wstring::const_iterator& iterator, Null& null);

    public:
      Object() = default;

      /**
       * @brief Constructs an @ref Object by parsing a JSON file from disk.
       * @param path  Path to the JSON file to load.
       * @throws FileError    if the file cannot be opened or read.
       * @throws ParsingError if the file content is not valid JSON.
       */
      Object(const std::filesystem::path& path);

      /**
       * @brief Constructs an @ref Object by parsing a JSON-encoded wide string.
       * @param text  The JSON wide string to parse.
       * @throws ParsingError if the text is not valid JSON.
       */
      Object(const std::wstring& text);

      Object(const Object&) = delete;
      Object(Object&&) = default;
      ~Object() override = default;

      Object& operator=(const Object&) = delete;
      Object& operator=(Object&&) = default;

      /**
       * @brief Returns Type::Object.
       * @return @ref Type::Object
       */
      Type type() const override;

      /**
       * @brief Returns a mutable reference to this object (self-reference).
       * @return Reference to *this.
       */
      Object& object() override;

      /**
       * @brief Returns a const reference to this object (self-reference).
       * @return Const reference to *this.
       */
      const Object& object() const override;

      /**
       * @brief Sets the value at @p key to the given JSON element (takes ownership).
       * @param key     The wide string key to associate the element with.
       * @param element The JSON element to store; ownership is transferred.
       */
      void set(const std::wstring& key, std::unique_ptr<Element>&& element);

      /**
       * @brief Sets the value at @p key to the given JSON object (move).
       * @param key    The wide string key.
       * @param object The JSON object to move-assign.
       */
      void set(const std::wstring& key, Object&& object);

      /**
       * @brief Sets the value at @p key to the given JSON array (move).
       * @param key   The wide string key.
       * @param array The JSON array to move-assign.
       */
      void set(const std::wstring& key, Array&& array);

      /**
       * @brief Sets the value at @p key to a copy of the given JSON number.
       * @param key    The wide string key.
       * @param number The JSON number to copy.
       */
      void set(const std::wstring& key, const Number& number);

      /**
       * @brief Sets the value at @p key to the given numeric value.
       * @param key    The wide string key.
       * @param number The double value to store.
       */
      void set(const std::wstring& key, double number);

      /**
       * @brief Sets the value at @p key to a copy of the given JSON string.
       * @param key    The wide string key.
       * @param string The JSON string to copy.
       */
      void set(const std::wstring& key, const String& string);

      /**
       * @brief Sets the value at @p key to the given JSON string (move).
       * @param key    The wide string key.
       * @param string The JSON string to move-assign.
       */
      void set(const std::wstring& key, String&& string);

      /**
       * @brief Sets the value at @p key to a copy of the given wide string.
       * @param key    The wide string key.
       * @param string The wide string value to copy.
       */
      void set(const std::wstring& key, const std::wstring& string);

      /**
       * @brief Sets the value at @p key to the given wide string (move).
       * @param key    The wide string key.
       * @param string The wide string value to move-assign.
       */
      void set(const std::wstring& key, std::wstring&& string);

      /**
       * @brief Sets the value at @p key to a copy of the given JSON boolean.
       * @param key   The wide string key.
       * @param value The JSON boolean to copy.
       */
      void set(const std::wstring& key, const Boolean& value);

      /**
       * @brief Sets the value at @p key to the given boolean value.
       * @param key   The wide string key.
       * @param value The bool value to store.
       */
      void set(const std::wstring& key, bool value);

      /**
       * @brief Sets the value at @p key to a JSON null.
       * @param key The wide string key to set to null.
       */
      void set(const std::wstring& key);

      /**
       * @brief Returns a mutable reference to the element associated with @p key.
       * @param key The wide string key to look up.
       * @return Reference to the element stored at @p key.
       * @throws BoundError if @p key is not present in the object.
       */
      Element& get(const std::wstring& key);

      /**
       * @brief Returns a const reference to the element associated with @p key.
       * @param key The wide string key to look up.
       * @return Const reference to the element stored at @p key.
       * @throws BoundError if @p key is not present in the object.
       */
      const Element& get(const std::wstring& key) const;

      /**
       * @brief Returns a mutable reference to the element associated with @p key.
       * @param key The wide string key to look up.
       * @return Reference to the element stored at @p key.
       * @throws BoundError if @p key is not present in the object.
       */
      Element& operator[](const std::wstring& key);

      /**
       * @brief Returns a const reference to the element associated with @p key.
       * @param key The wide string key to look up.
       * @return Const reference to the element stored at @p key.
       * @throws BoundError if @p key is not present in the object.
       */
      const Element& operator[](const std::wstring& key) const;

      /**
       * @brief Removes the element associated with @p key from the object.
       * @param key The wide string key to remove.
       */
      void        unset(const std::wstring& key);

      /**
       * @brief Checks whether the object has no entries.
       * @return true if the object contains no key-value pairs, false otherwise.
       */
      bool        empty() const;

      /**
       * @brief Returns the number of key-value pairs in the object.
       * @return The entry count.
       */
      std::size_t size() const;

      /**
       * @brief Removes all entries from the object.
       */
      void        clear();

      /**
       * @brief Checks whether @p key is present in the object.
       * @param key The wide string key to search for.
       * @return true if @p key exists, false otherwise.
       */
      bool        contains(const std::wstring& key) const;

      /// Iterator type over the internal key-value map.
      using iterator = std::unordered_map<std::wstring, std::unique_ptr<Element>>::iterator;

      /// Const iterator type over the internal key-value map.
      using const_iterator = std::unordered_map<std::wstring, std::unique_ptr<Element>>::const_iterator;

      /**
       * @brief Returns an iterator to the first entry of the object.
       * @return Mutable begin iterator.
       */
      iterator       begin();

      /**
       * @brief Returns a const iterator to the first entry of the object.
       * @return Const begin iterator.
       */
      const_iterator begin() const;

      /**
       * @brief Returns a const iterator to the first entry of the object.
       * @return Const begin iterator.
       */
      const_iterator cbegin() const;

      /**
       * @brief Returns an iterator past the last entry of the object.
       * @return Mutable end iterator.
       */
      iterator       end();

      /**
       * @brief Returns a const iterator past the last entry of the object.
       * @return Const end iterator.
       */
      const_iterator end() const;

      /**
       * @brief Returns a const iterator past the last entry of the object.
       * @return Const end iterator.
       */
      const_iterator cend() const;

      /**
       * @brief Serializes this JSON object to its string representation.
       * @return A wide string containing the JSON-encoded object (e.g. `{"key":value,...}`).
       */
      std::wstring stringify() const override;
    };

    /**
     * @brief Represents a JSON array: an ordered list of JSON elements.
     *
     * Elements are stored by unique pointer and accessed by zero-based index.
     * Supports appending, replacing, removing and iterating elements.
     */
    class Array : public Element
    {
    private:
      std::vector<std::unique_ptr<Element>> _vector; ///< Internal ordered storage of JSON elements.

    public:
      Array() = default;
      Array(const Array&) = delete;
      Array(Array&&) = default;
      ~Array() override = default;

      Array& operator=(const Array&) = delete;
      Array& operator=(Array&&) = default;

      /**
       * @brief Returns Type::Array.
       * @return @ref Type::Array
       */
      Type type() const override;

      /**
       * @brief Returns a mutable reference to this array (self-reference).
       * @return Reference to *this.
       */
      Array& array() override;

      /**
       * @brief Returns a const reference to this array (self-reference).
       * @return Const reference to *this.
       */
      const Array& array() const override;

      /**
       * @brief Appends a JSON element to the end of the array (takes ownership).
       * @param element The JSON element to push back; ownership is transferred.
       */
      void push(std::unique_ptr<Element>&& element);

      /**
       * @brief Appends a JSON object to the end of the array (move).
       * @param object The JSON object to move-append.
       */
      void push(Object&& object);

      /**
       * @brief Appends a JSON array to the end of this array (move).
       * @param array The JSON array to move-append.
       */
      void push(Array&& array);

      /**
       * @brief Appends a copy of the given JSON number to the end of the array.
       * @param number The JSON number to copy and append.
       */
      void push(const Number& number);

      /**
       * @brief Appends the given numeric value to the end of the array.
       * @param number The double value to append.
       */
      void push(double number);

      /**
       * @brief Appends a copy of the given JSON string to the end of the array.
       * @param string The JSON string to copy and append.
       */
      void push(const String& string);

      /**
       * @brief Appends a JSON string to the end of the array (move).
       * @param string The JSON string to move-append.
       */
      void push(String&& string);

      /**
       * @brief Appends a copy of the given wide string to the end of the array.
       * @param string The wide string value to copy and append.
       */
      void push(const std::wstring& string);

      /**
       * @brief Appends a wide string to the end of the array (move).
       * @param string The wide string value to move-append.
       */
      void push(std::wstring&& string);

      /**
       * @brief Appends a copy of the given JSON boolean to the end of the array.
       * @param value The JSON boolean to copy and append.
       */
      void push(const Boolean& value);

      /**
       * @brief Appends the given boolean value to the end of the array.
       * @param value The bool value to append.
       */
      void push(bool value);

      /**
       * @brief Appends a JSON null element to the end of the array.
       */
      void push();

      /**
       * @brief Replaces the element at @p position with a JSON element (takes ownership).
       * @param position The zero-based index to replace.
       * @param element  The new JSON element; ownership is transferred.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, std::unique_ptr<Element>&& element);

      /**
       * @brief Replaces the element at @p position with a JSON object (move).
       * @param position The zero-based index to replace.
       * @param object   The JSON object to move-assign.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, Object&& object);

      /**
       * @brief Replaces the element at @p position with a JSON array (move).
       * @param position The zero-based index to replace.
       * @param array    The JSON array to move-assign.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, Array&& array);

      /**
       * @brief Replaces the element at @p position with a copy of the given JSON number.
       * @param position The zero-based index to replace.
       * @param number   The JSON number to copy.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, const Number& number);

      /**
       * @brief Replaces the element at @p position with the given numeric value.
       * @param position The zero-based index to replace.
       * @param number   The double value to store.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, double number);

      /**
       * @brief Replaces the element at @p position with a copy of the given JSON string.
       * @param position The zero-based index to replace.
       * @param string   The JSON string to copy.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, const String& string);

      /**
       * @brief Replaces the element at @p position with a JSON string (move).
       * @param position The zero-based index to replace.
       * @param string   The JSON string to move-assign.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, String&& string);

      /**
       * @brief Replaces the element at @p position with a copy of the given wide string.
       * @param position The zero-based index to replace.
       * @param string   The wide string value to copy.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, const std::wstring& string);

      /**
       * @brief Replaces the element at @p position with a wide string (move).
       * @param position The zero-based index to replace.
       * @param string   The wide string value to move-assign.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, std::wstring&& string);

      /**
       * @brief Replaces the element at @p position with a copy of the given JSON boolean.
       * @param position The zero-based index to replace.
       * @param value    The JSON boolean to copy.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, const Boolean& value);

      /**
       * @brief Replaces the element at @p position with the given boolean value.
       * @param position The zero-based index to replace.
       * @param value    The bool value to store.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position, bool value);

      /**
       * @brief Replaces the element at @p position with a JSON null.
       * @param position The zero-based index to set to null.
       * @throws BoundError if @p position is out of range.
       */
      void set(std::size_t position);

      /**
       * @brief Returns a mutable reference to the element at @p position.
       * @param position The zero-based index to access.
       * @return Reference to the element at @p position.
       * @throws BoundError if @p position is out of range.
       */
      Element& get(std::size_t position);

      /**
       * @brief Returns a const reference to the element at @p position.
       * @param position The zero-based index to access.
       * @return Const reference to the element at @p position.
       * @throws BoundError if @p position is out of range.
       */
      const Element& get(std::size_t position) const;

      /**
       * @brief Returns a mutable reference to the element at @p position.
       * @param position The zero-based index to access.
       * @return Reference to the element at @p position.
       * @throws BoundError if @p position is out of range.
       */
      Element& operator[](std::size_t position);

      /**
       * @brief Returns a const reference to the element at @p position.
       * @param position The zero-based index to access.
       * @return Const reference to the element at @p position.
       * @throws BoundError if @p position is out of range.
       */
      const Element& operator[](std::size_t position) const;

      /**
       * @brief Removes the element at @p position from the array.
       * @param position The zero-based index of the element to remove.
       * @throws BoundError if @p position is out of range.
       */
      void        unset(std::size_t position);

      /**
       * @brief Checks whether the array contains no elements.
       * @return true if the array is empty, false otherwise.
       */
      bool        empty() const;

      /**
       * @brief Returns the number of elements in the array.
       * @return The element count.
       */
      std::size_t size() const;

      /**
       * @brief Resizes the array to exactly @p size elements.
       *
       * If @p size is greater than the current size, new null elements are appended.
       * If smaller, elements beyond @p size are removed.
       *
       * @param size The desired number of elements.
       */
      void        resize(std::size_t size);

      /**
       * @brief Pre-allocates internal storage for at least @p size elements.
       *
       * Does not change the logical size of the array.
       *
       * @param size The number of elements to reserve space for.
       */
      void        reserve(std::size_t size);

      /**
       * @brief Removes all elements from the array.
       */
      void        clear();

      /// Iterator type over the internal element vector.
      using iterator = std::vector<std::unique_ptr<Element>>::iterator;

      /// Const iterator type over the internal element vector.
      using const_iterator = std::vector<std::unique_ptr<Element>>::const_iterator;

      /**
       * @brief Returns an iterator to the first element of the array.
       * @return Mutable begin iterator.
       */
      iterator       begin();

      /**
       * @brief Returns a const iterator to the first element of the array.
       * @return Const begin iterator.
       */
      const_iterator begin() const;

      /**
       * @brief Returns an iterator past the last element of the array.
       * @return Mutable end iterator.
       */
      iterator       end();

      /**
       * @brief Returns a const iterator past the last element of the array.
       * @return Const end iterator.
       */
      const_iterator end() const;

      /**
       * @brief Serializes this JSON array to its string representation.
       * @return A wide string containing the JSON-encoded array (e.g. `[value,...]`).
       */
      std::wstring stringify() const override;
    };

    /**
     * @brief Represents a JSON number as a double-precision floating-point value.
     */
    class Number : public Element
    {
    public:
      double value; ///< The numeric value.

      /**
       * @brief Constructs a @ref Number with a default value of zero.
       */
      Number();

      /**
       * @brief Constructs a @ref Number with the given value.
       * @param value The double value to store.
       */
      Number(double value);

      Number(const Number&) = default;
      Number(Number&&) = default;
      ~Number() override = default;

      Number& operator=(const Number&) = default;
      Number& operator=(Number&&) = default;

      /**
       * @brief Returns Type::Number.
       * @return @ref Type::Number
       */
      Type type() const override;

      /**
       * @brief Returns a mutable reference to the numeric value.
       * @return Reference to @ref value.
       */
      double& number() override;

      /**
       * @brief Returns the numeric value by copy.
       * @return The double value held by @ref value.
       */
      double  number() const override;

      /**
       * @brief Serializes this JSON number to its string representation.
       * @return A wide string containing the JSON-encoded number.
       */
      std::wstring stringify() const override;
    };

    /**
     * @brief Represents a JSON string as a wide character string.
     */
    class String : public Element
    {
    public:
      std::wstring value; ///< The string value.

      String() = default;
      String(const String&) = default;

      /**
       * @brief Constructs a @ref String by copying the given wide string.
       * @param value The wide string to copy.
       */
      String(const std::wstring& value);

      /**
       * @brief Constructs a @ref String by moving the given wide string.
       * @param value The wide string to move.
       */
      String(std::wstring&& value);

      String(String&&) = default;
      ~String() override = default;

      String& operator=(const String&) = default;
      String& operator=(String&&) = default;

      /**
       * @brief Returns Type::String.
       * @return @ref Type::String
       */
      Type type() const override;

      /**
       * @brief Returns a mutable reference to the string value.
       * @return Reference to @ref value.
       */
      std::wstring& string() override;

      /**
       * @brief Returns a const reference to the string value.
       * @return Const reference to @ref value.
       */
      const std::wstring& string() const override;

      /**
       * @brief Serializes this JSON string to its string representation.
       * @return A wide string containing the JSON-encoded string (with surrounding quotes and escape sequences).
       */
      std::wstring stringify() const override;
    };

    /**
     * @brief Represents a JSON boolean value (true or false).
     */
    class Boolean : public Element
    {
    public:
      bool value; ///< The boolean value.

      /**
       * @brief Constructs a @ref Boolean with a default value of false.
       */
      Boolean();

      /**
       * @brief Constructs a @ref Boolean with the given value.
       * @param value The bool value to store.
       */
      Boolean(bool value);

      Boolean(const Boolean&) = default;
      Boolean(Boolean&&) = default;
      ~Boolean() override = default;

      Boolean& operator=(const Boolean&) = default;
      Boolean& operator=(Boolean&&) = default;

      /**
       * @brief Returns Type::Boolean.
       * @return @ref Type::Boolean
       */
      Type type() const override;

      /**
       * @brief Returns a mutable reference to the boolean value.
       * @return Reference to @ref value.
       */
      bool& boolean() override;

      /**
       * @brief Returns the boolean value by copy.
       * @return The bool value held by @ref value.
       */
      bool  boolean() const override;

      /**
       * @brief Serializes this JSON boolean to its string representation.
       * @return Either `L"true"` or `L"false"`.
       */
      std::wstring stringify() const override;
    };

    /**
     * @brief Represents a JSON null value.
     */
    class Null : public Element
    {
    public:
      Null() = default;
      Null(const Null&) = default;
      Null(Null&&) = default;
      ~Null() override = default;

      Null& operator=(const Null&) = default;
      Null& operator=(Null&&) = default;

      /**
       * @brief Returns Type::Null.
       * @return @ref Type::Null
       */
      Type type() const override;

      /**
       * @brief Returns true, since this element is always a null.
       * @return true
       */
      bool null() const override;

      /**
       * @brief Serializes this JSON null to its string representation.
       * @return `L"null"`
       */
      std::wstring stringify() const override;
    };

    /**
     * @brief Exception thrown when a typed accessor is called on an element of the wrong type.
     *
     * For example, calling `number()` on a @ref String element will throw this exception.
     */
    class TypeError : public std::runtime_error
    {
    public:
      /**
       * @brief Constructs a @ref TypeError with the given message.
       * @param message A description of the type mismatch.
       */
      TypeError(const std::string& message);
      ~TypeError() = default;
    };

    /**
     * @brief Exception thrown when an out-of-range index or missing key is accessed.
     *
     * Raised by @ref Array and @ref Object accessors when the requested position or key does not exist.
     */
    class BoundError : public std::runtime_error
    {
    public:
      /**
       * @brief Constructs a @ref BoundError with the given message.
       * @param message A description of the out-of-bound access.
       */
      BoundError(const std::string& message);
      ~BoundError() = default;
    };

    /**
     * @brief Exception thrown when a JSON file cannot be opened or read.
     */
    class FileError : public std::runtime_error
    {
    public:
      /**
       * @brief Constructs a @ref FileError with the given message.
       * @param message A description of the file I/O failure.
       */
      FileError(const std::string& message);
      ~FileError() = default;
    };

    /**
     * @brief Exception thrown when JSON parsing fails due to malformed input.
     */
    class ParsingError : public std::runtime_error
    {
    public:
      /**
       * @brief Constructs a @ref ParsingError with the given message.
       * @param message A description of the parsing failure.
       */
      ParsingError(const std::string& message);
      ~ParsingError() = default;
    };
  };

  /// @brief Convenience alias for the JavaScriptObjectNotation namespace.
  namespace JSON = JavaScriptObjectNotation;
}

/**
 * @brief Writes the JSON serialization of @p json to the wide output stream @p stream.
 * @param stream The wide output stream to write to.
 * @param json   The JSON element to serialize.
 * @return Reference to @p stream for chaining.
 */
std::wostream& operator<<(std::wostream& stream, const Game::JSON::Element& json);