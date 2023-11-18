﻿// This file is part of YamlDotNet - A .NET library for YAML.
// Copyright (c) Antoine Aubry and contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
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

using System.Collections.Generic;
using Xunit;
using YamlDotNet.Serialization;
using YamlDotNet.Serialization.Callbacks;

namespace YamlDotNet.Test.Analyzers.StaticGenerator
{
    public class ObjectTests
    {
        [Fact]
        public void InheritedMembersWorks()
        {
            var deserializer = new StaticDeserializerBuilder(new StaticContext()).Build();
            var yaml = @"NotInherited: hello
Inherited: world
";
            var actual = deserializer.Deserialize<SerializedInheritedClass>(yaml);
            Assert.Equal("hello", actual.NotInherited);
            Assert.Equal("world", actual.Inherited);
            var serializer = new StaticSerializerBuilder(new StaticContext()).Build();
            var actualYaml = serializer.Serialize(actual);
            Assert.Equal(yaml.NormalizeNewLines().TrimNewLines(), actualYaml.NormalizeNewLines().TrimNewLines());
        }

        [Fact]
        public void RegularObjectWorks()
        {
            var deserializer = new StaticDeserializerBuilder(new StaticContext()).Build();
            var yaml = @"Prop1: hello
Prop2: 1
Hello: world
Inner:
  Prop1: a
  Prop2: 2
Nested:
  NestedProp: abc
DictionaryOfArrays:
  a:
  - 1
  b:
  - 2
SomeValue: ""abc""
SomeDictionary:
  a: 1
  b: 2
";
            var actual = deserializer.Deserialize<RegularObjectOuter>(yaml);
            Assert.Equal("hello", actual.Prop1);
            Assert.Equal(1, actual.Prop2);
            Assert.Equal("world", actual.Member);
            Assert.Equal("I am ignored", actual.Ignored);
            Assert.NotNull(actual.Inner);
            Assert.Equal("a", actual.Inner.Prop1);
            Assert.Equal(2, actual.Inner.Prop2);
            Assert.NotNull(actual.Nested);
            Assert.Equal("abc", actual.Nested.NestedProp);
            Assert.Equal("1", actual.DictionaryOfArrays["a"][0]);
            Assert.Equal("2", actual.DictionaryOfArrays["b"][0]);
            Assert.Equal("abc", actual.SomeValue);
            Assert.Equal("1", ((IDictionary<object, object>)actual.SomeDictionary)["a"]);
            Assert.Equal("2", ((IDictionary<object, object>)actual.SomeDictionary)["b"]);

            var serializer = new StaticSerializerBuilder(new StaticContext()).Build();
            var actualYaml = serializer.Serialize(actual);
            yaml = @"Prop1: hello
Prop2: 1
# A Description
Hello: ""world""
Inner:
  Prop1: a
  Prop2: 2
Nested:
  NestedProp: abc
DictionaryOfArrays:
  a:
  - 1
  b:
  - 2
SomeValue: abc
SomeDictionary:
  a: 1
  b: 2";
            Assert.Equal(yaml.NormalizeNewLines().TrimNewLines(), actualYaml.NormalizeNewLines().TrimNewLines());
        }

        [Fact]
        public void CallbacksAreExecuted()
        {
            var yaml = "Test: Hi";
            var deserializer = new StaticDeserializerBuilder(new StaticContext()).Build();
            var test = deserializer.Deserialize<TestState>(yaml);

            Assert.Equal(1, test.OnDeserializedCallCount);
            Assert.Equal(1, test.OnDeserializingCallCount);

            var serializer = new StaticSerializerBuilder(new StaticContext()).Build();
            yaml = serializer.Serialize(test);
            Assert.Equal(1, test.OnSerializedCallCount);
            Assert.Equal(1, test.OnSerializingCallCount);
        }

        [YamlSerializable]
        public class TestState
        {
            public int OnDeserializedCallCount { get; set; }
            public int OnDeserializingCallCount { get; set; }
            public int OnSerializedCallCount { get; set; }
            public int OnSerializingCallCount { get; set; }

            public string Test { get; set; } = string.Empty;

            [OnDeserialized]
            public void Deserialized() => OnDeserializedCallCount++;

            [OnDeserializing]
            public void Deserializing() => OnDeserializingCallCount++;

            [OnSerialized]
            public void Serialized() => OnSerializedCallCount++;

            [OnSerializing]
            public void Serializing() => OnSerializingCallCount++;
        }

    }
    public class InheritedClass
    {
        public string Inherited { get; set; }
    }

    [YamlSerializable]
    public class SerializedInheritedClass : InheritedClass
    {
        public string NotInherited { get; set; }
    }

    [YamlSerializable]
    public class RegularObjectOuter
    {
        public string Prop1 { get; set; }
        public int Prop2 { get; set; }
        [YamlMember(Alias = "Hello", Description = "A Description", ScalarStyle = YamlDotNet.Core.ScalarStyle.DoubleQuoted)]
        public string Member { get; set; }
        [YamlIgnore]
        public string Ignored { get; set; } = "I am ignored";
        public RegularObjectInner Inner { get; set; }
        public NestedClass Nested { get; set; }

        public Dictionary<string, string[]> DictionaryOfArrays { get; set; }

        public object SomeValue { get; set; }

        public object SomeDictionary { get; set; }

        [YamlSerializable]
        public class NestedClass
        {
            public string NestedProp { get; set; }
        }
    }

    [YamlSerializable]
    public class RegularObjectInner
    {
        public string Prop1 { get; set; }
        public int Prop2 { get; set; }
    }
}
