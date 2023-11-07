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
  
  namespace JSON
  {
    // Forward declaration of JSON types
    class Object;
    class Array;
    class Number;
    class String;
    class Boolean;
    class Null;

    enum Type
    {
      TypeObject,   // A collection of name–value pairs
      TypeArray,    // An ordered list of zero or more elements
      TypeNumber,   // A signed decimal number
      TypeString,   // A sequence of zero or more characters
      TypeBoolean,  // Either of the values true or false
      TypeNull      // An empty value
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
      virtual Game::JSON::Type  type() const = 0;

      // Return true if null type
      virtual bool null() const;

      // Non-const accessor, throw WrongType exception when wrong type accessed
      virtual Game::JSON::Object&   object();
      virtual Game::JSON::Array&    array();
      virtual Game::JSON::Number&   number();
      virtual Game::JSON::String&   string();
      virtual Game::JSON::Boolean&  boolean();

      // Const accessor, throw WrongType exception when wrong type accessed
      virtual const Game::JSON::Object&   object() const;
      virtual const Game::JSON::Array&    array() const;
      virtual const Game::JSON::Number&   number() const;
      virtual const Game::JSON::String&   string() const;
      virtual const Game::JSON::Boolean&  boolean() const;

      // Write JSON to a string
      virtual std::string stringify() const = 0;
    };

    class Object : public Element
    {
    private:
      void                                  loadWhitespaces(const std::string& text, std::string::const_iterator& iterator);  // Skip whitespaces
      std::unique_ptr<Game::JSON::Element>  loadElement(const std::string& text, std::string::const_iterator& iterator);      // Load JSON element

      void  loadObject(const std::string& text, std::string::const_iterator& iterator, Game::JSON::Object& object);
      void  loadArray(const std::string& text, std::string::const_iterator& iterator, Game::JSON::Array& array);
      void  loadNumber(const std::string& text, std::string::const_iterator& iterator, Game::JSON::Number& number);
      void  loadString(const std::string& text, std::string::const_iterator& iterator, Game::JSON::String& string);
      void  loadBoolean(const std::string& text, std::string::const_iterator& iterator, Game::JSON::Boolean& boolean);
      void  loadNull(const std::string& text, std::string::const_iterator& iterator, Game::JSON::Null& null);

    public:
      std::unordered_map<std::string, std::unique_ptr<Game::JSON::Element>> map;  // Name-value pairs collection

      Object() = default;
      Object(const Object&) = delete;
      Object(std::unordered_map<std::string, std::unique_ptr<Game::JSON::Element>>&& map);
      Object(Object&&) = default;
      Object(const std::filesystem::path& path);
      ~Object() override = default;

      Object& operator=(const Object&) = delete;
      Object& operator=(Object&&) = default;

      Game::JSON::Type type() const override;

      Game::JSON::Object&       object() override;
      const Game::JSON::Object& object() const override;
      
      void  load(const std::filesystem::path& path);        // Clear JSON object and load given JSON file, throw errors
      void  save(const std::filesystem::path& path) const;  // Save JSON object to file, throw error

      std::string stringify() const override;
    };

    class Array : public Element
    {
    public:
      std::vector<std::unique_ptr<Game::JSON::Element>> vector; // Array of JSON elements

      Array() = default;
      Array(const Array&) = delete;
      Array(std::vector<std::unique_ptr<Game::JSON::Element>>&& vector);
      Array(Array&&) = default;
      ~Array() override = default;

      Array& operator=(const Array&) = delete;
      Array& operator=(Array&&) = default;

      Game::JSON::Type type() const override;

      Game::JSON::Array&        array() override;
      const Game::JSON::Array&  array() const override;
      
      std::string stringify() const override;
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

      Game::JSON::Type type() const override;

      Game::JSON::Number&       number() override;
      const Game::JSON::Number& number() const override;
      
      std::string stringify() const override;
    };

    class String : public Element
    {
    public:
      std::string value;

      String() = default;
      String(const std::string& value);
      String(const String&) = default;
      String(std::string&& value);
      String(String&&) = default;
      ~String() override = default;

      String& operator=(const String&) = default;
      String& operator=(String&&) = default;

      Game::JSON::Type type() const override;

      Game::JSON::String&       string() override;
      const Game::JSON::String& string() const override;
      
      std::string stringify() const override;
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

      Game::JSON::Type type() const override;

      Game::JSON::Boolean&        boolean() override;
      const Game::JSON::Boolean&  boolean() const override;
      
      std::string stringify() const override;
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

      Game::JSON::Type type() const override;

      bool null() const override;
      
      std::string stringify() const override;
    };

    class TypeError : public std::runtime_error
    {
    public:
      TypeError(const std::string& message);
      ~TypeError() = default;
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
  }
}

// Write JSON to stream
std::ostream& operator<<(std::ostream& stream, Game::JSON::Element& json);
std::wostream& operator<<(std::wostream& stream, Game::JSON::Element& json);