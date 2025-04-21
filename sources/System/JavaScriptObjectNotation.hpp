#pragma once

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
  namespace JavaScriptObjectNotation
  {
    // Forward declaration of JSON types
    class Object;
    class Array;
    class Number;
    class String;
    class Boolean;
    class Null;

    enum class Type
    {
      Object,  // A collection of name–value pairs
      Array,   // An ordered list of zero or more elements
      Number,  // A signed decimal number
      String,  // A sequence of zero or more characters
      Boolean, // Either of the values true or false
      Null     // An empty value
    };

    // JSON abstract interface
    class Element
    {
    public:
      Element() = default;
      Element(const Element&) = default;
      Element(Element&&) = default;
      virtual ~Element() = 0;

      Element& operator=(const Element&) = default;
      Element& operator=(Element&&) = default;

      // Get the type of the JSON element
      virtual Type  type() const = 0;

      // Return true if null type
      virtual bool null() const;

      // Non-const accessor, throw WrongType exception when wrong type accessed
      virtual Object&       object();
      virtual Array&        array();
      virtual double&       number();
      virtual std::wstring& string();
      virtual bool&         boolean();

      // Const accessor, throw WrongType exception when wrong type accessed
      virtual const Object&       object() const;
      virtual const Array&        array() const;
      virtual double              number() const;
      virtual const std::wstring& string() const;
      virtual bool                boolean() const;

      // Write JSON to a string
      virtual std::wstring stringify() const = 0;
    };

    class Object : public Element
    {
    private:
      std::unordered_map<std::wstring, std::unique_ptr<Element>> _map; // Name-value pairs collection (NOTE: private this)

      void load(const std::wstring& text);

      static void                     loadWhitespaces(const std::wstring& text, std::wstring::const_iterator& iterator);
      static std::unique_ptr<Element> loadElement(const std::wstring& text, std::wstring::const_iterator& iterator);
      
      static void loadObject(const std::wstring& text, std::wstring::const_iterator& iterator, Object& object);
      static void loadArray(const std::wstring& text, std::wstring::const_iterator& iterator, Array& array);
      static void loadNumber(const std::wstring& text, std::wstring::const_iterator& iterator, Number& number);
      static void loadString(const std::wstring& text, std::wstring::const_iterator& iterator, String& string);
      static void loadBoolean(const std::wstring& text, std::wstring::const_iterator& iterator, Boolean& boolean);
      static void loadNull(const std::wstring& text, std::wstring::const_iterator& iterator, Null& null);

    public:
      Object() = default;
      Object(const std::filesystem::path& path);
      Object(const std::wstring& text);
      Object(const Object&) = delete;
      Object(Object&&) = default;
      ~Object() override = default;

      Object& operator=(const Object&) = delete;
      Object& operator=(Object&&) = default;

      Type type() const override;

      Object&       object() override;
      const Object& object() const override;

      void set(const std::wstring& key, std::unique_ptr<Element>&& element); // Set value at key to JSON element
      void set(const std::wstring& key, Object&& object);                    // Set value at key to JSON object
      void set(const std::wstring& key, Array&& array);                      // Set value at key to JSON array
      void set(const std::wstring& key, const Number& number);               // Set value at key to JSON number
      void set(const std::wstring& key, double number);                      // Set value at key to number
      void set(const std::wstring& key, const String& string);               // Set value at key to JSON string
      void set(const std::wstring& key, String&& string);                    // Set value at key to JSON string
      void set(const std::wstring& key, const std::wstring& string);         // Set value at key to string
      void set(const std::wstring& key, std::wstring&& string);              // Set value at key to string
      void set(const std::wstring& key, const Boolean& value);               // Set value at key to JSON boolean
      void set(const std::wstring& key, bool value);                         // Set value at key to boolean
      void set(const std::wstring& key);                                     // Set value at key to JSON null

      Element&       get(const std::wstring& key);              // Get JSON element at key
      const Element& get(const std::wstring& key) const;        // Get JSON element at key
      Element&       operator[](const std::wstring& key);       // Get JSON element at key
      const Element& operator[](const std::wstring& key) const; // Get JSON element at key

      void        unset(const std::wstring& key);          // Remove element from JSON object
      bool        empty() const;                           // Check if JSON object is empty
      std::size_t size() const;                            // Get the number of entry in JSON object
      void        clear();                                 // Reset JSON object
      bool        contains(const std::wstring& key) const; // Check if key is in JSON object

      using iterator = std::unordered_map<std::wstring, std::unique_ptr<Element>>::iterator;
      using const_iterator = std::unordered_map<std::wstring, std::unique_ptr<Element>>::const_iterator;

      iterator       begin();       // Get begin iterator
      const_iterator begin() const; // Get begin const iterator
      iterator       end();         // Get end iterator
      const_iterator end() const;   // Get end const iterator

      std::wstring stringify() const override;
    };

    class Array : public Element
    {
    private:
      std::vector<std::unique_ptr<Element>> _vector; // Array of JSON elements (NOTE: private this)

    public:
      Array() = default;
      Array(const Array&) = delete;
      Array(Array&&) = default;
      ~Array() override = default;

      Array& operator=(const Array&) = delete;
      Array& operator=(Array&&) = default;

      Type type() const override;

      Array&       array() override;
      const Array& array() const override;

      void push(std::unique_ptr<Element>&& element); // Push back a JSON element
      void push(Object&& object);                    // Push back a JSON object
      void push(Array&& array);                      // Push back a JSON array
      void push(const Number& number);               // Push back a JSON number
      void push(double number);                      // Push back a number
      void push(const String& string);               // Push back a JSON string
      void push(String&& string);                    // Push back a JSON string
      void push(const std::wstring& string);         // Push back a string
      void push(std::wstring&& string);              // Push back a string
      void push(const Boolean& value);               // Push back a JSON boolean
      void push(bool value);                         // Push back a boolean
      void push();                                   // Push back a JSON null
      
      void set(std::size_t position, std::unique_ptr<Element>&& element); // Set a JSON element at position
      void set(std::size_t position, Object&& object);                    // Set value at position to a JSON object
      void set(std::size_t position, Array&& array);                      // Set value at position to a JSON array
      void set(std::size_t position, const Number& number);               // Set value at position to a JSON number
      void set(std::size_t position, double number);                      // Set value at position to a number
      void set(std::size_t position, const String& string);               // Set value at position to a JSON string
      void set(std::size_t position, String&& string);                    // Set value at position to a JSON string
      void set(std::size_t position, const std::wstring& string);         // Set value at position to a string
      void set(std::size_t position, std::wstring&& string);              // Set value at position to a string
      void set(std::size_t position, const Boolean& value);               // Set value at position to a JSON boolean
      void set(std::size_t position, bool value);                         // Set value at position to a boolean
      void set(std::size_t position);                                     // Set value at position to a JSON null

      Element&       get(std::size_t position);              // Get JSON element at position
      const Element& get(std::size_t position) const;        // Get JSON element at position
      Element&       operator[](std::size_t position);       // Get JSON element at position
      const Element& operator[](std::size_t position) const; // Get JSON element at position

      void        unset(std::size_t position); // Remove element from JSON array
      bool        empty() const;               // Check if array is empty
      std::size_t size() const;                // Get the size of the JSON array
      void        resize(std::size_t size);    // Resize the JSON array
      void        reserve(std::size_t size);   // Pre-allocate JSON array
      void        clear();                     // Reset JSON array

      using iterator = std::vector<std::unique_ptr<Element>>::iterator;
      using const_iterator = std::vector<std::unique_ptr<Element>>::const_iterator;

      iterator       begin();       // Get begin iterator
      const_iterator begin() const; // Get begin const iterator
      iterator       end();         // Get end iterator
      const_iterator end() const;   // Get end const iterator

      std::wstring stringify() const override;
    };

    class Number : public Element
    {
    public:
      double value;

      Number();
      Number(double value);
      Number(const Number&) = default;
      Number(Number&&) = default;
      ~Number() override = default;

      Number& operator=(const Number&) = default;
      Number& operator=(Number&&) = default;

      Type type() const override;

      double& number() override;
      double  number() const override;
      
      std::wstring stringify() const override;
    };

    class String : public Element
    {
    public:
      std::wstring value;

      String() = default;
      String(const String&) = default;
      String(const std::wstring& value);
      String(std::wstring&& value);
      String(String&&) = default;
      ~String() override = default;

      String& operator=(const String&) = default;
      String& operator=(String&&) = default;

      Type type() const override;

      std::wstring&       string() override;
      const std::wstring& string() const override;
      
      std::wstring stringify() const override;
    };

    class Boolean : public Element
    {
    public:
      bool value;

      Boolean();
      Boolean(bool value);
      Boolean(const Boolean&) = default;
      Boolean(Boolean&&) = default;
      ~Boolean() override = default;

      Boolean& operator=(const Boolean&) = default;
      Boolean& operator=(Boolean&&) = default;

      Type type() const override;

      bool& boolean() override;
      bool  boolean() const override;
      
      std::wstring stringify() const override;
    };

    class Null : public Element
    {
    public:
      Null() = default;
      Null(const Null&) = default;
      Null(Null&&) = default;
      ~Null() override = default;

      Null& operator=(const Null&) = default;
      Null& operator=(Null&&) = default;

      Type type() const override;

      bool null() const override;
      
      std::wstring stringify() const override;
    };

    class TypeError : public std::runtime_error
    {
    public:
      TypeError(const std::string& message);
      ~TypeError() = default;
    };

    class BoundError : public std::runtime_error
    {
    public:
      BoundError(const std::string& message);
      ~BoundError() = default;
    };

    class FileError : public std::runtime_error
    {
    public:
      FileError(const std::string& message);
      ~FileError() = default;
    };

    class ParsingError : public std::runtime_error
    {
    public:
      ParsingError(const std::string& message);
      ~ParsingError() = default;
    };
  };

  namespace JSON = JavaScriptObjectNotation;
}

// Write JSON to stream
std::wostream& operator<<(std::wostream& stream, const Game::JSON::Element& json);