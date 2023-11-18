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

using System;
using System.Collections.Generic;
using System.IO;
using YamlDotNet.Serialization;
using YamlDotNet.Serialization.NamingConventions;
using YamlDotNet.Test.Serialization;

namespace YamlDotNet.AotTest
{
    class Program
    {
        static int Main()
        {
            Console.WriteLine();
            Console.WriteLine("\x1b[37m---------------------------------------------------------------------------------"); 
            Console.WriteLine();

            Console.WriteLine("\x1b[97m Running AOT tests...");
            Console.WriteLine();

            TryDeserialize<MyDictionary>("DictionaryNodeDeserializer", "myDictionary: { winners: 3 }");
            TryDeserialize<MyList>("CollectionNodeDeserializer", "myList: [ 1, 2, 3 ]");
            TryDeserialize<MyArray>("ArayNodeDeserializer", "myArray: [ 1, 2, 3 ]");
            TrySerialize("TraverseGenericDictionary", new GenericTestDictionary<long, long> { { 1, 2 } });

            Console.WriteLine();
            Console.WriteLine(" \x1b[93m{0}\x1b[97m test succeeded, \x1b[93m{1}\x1b[97m tests failed", SucceededTestCount, FailedTestCount);

            Console.WriteLine();
            Console.WriteLine("\x1b[37m---------------------------------------------------------------------------------");
            Console.WriteLine("\x1b[0m");

            return FailedTestCount;
        }

        private static int SucceededTestCount;
        private static int FailedTestCount;

        private static void TrySerialize<T>(string testName, T graph)
        {
            var output = new StringWriter();
            var serializer = new SerializerBuilder()
                .WithNamingConvention(CamelCaseNamingConvention.Instance)
                .Build();
            PerformTest(testName, () => serializer.Serialize(output, graph));
        }

        private static void TryDeserialize<T>(string testName, string yaml)
        {
            var input = new StringReader(yaml);
            var deserializer = new DeserializerBuilder()
                .WithNamingConvention(CamelCaseNamingConvention.Instance)
                .Build();
            PerformTest(testName, () => deserializer.Deserialize<T>(input));
        }

        private static void PerformTest(string testName, Action act)
        {
            Console.Write("\x1b[37m ");
            Console.Write(testName.PadRight(70));

            try
            {
                act();
                Console.WriteLine("\x1b[92m[success]\x1b[37m");
                ++SucceededTestCount;
            }
            catch (Exception ex)
            {
                var current = ex;
                while (current != null)
                {
#pragma warning disable CS0618 // Type or member is obsolete
                    if (current is ExecutionEngineException)
#pragma warning restore CS0618 // Type or member is obsolete
                    {
                        Console.WriteLine("\x1b[91m[failure]");
                        Console.Write("\x1b[93m ");
                        Console.WriteLine(current.Message);
                        Console.Write("\x1b[37m");
                        ++FailedTestCount;
                        return;
                    }

                    current = current.InnerException;
                }
                Console.Write("\x1b[91m");
                throw;
            }
        }
    }

#pragma warning disable IDE1006 // Naming Styles
    public class MyDictionary
    {
        public Dictionary<string, int> myDictionary { get; set; }
    }

    public class MyList
    {
        public List<int> myList { get; set; }
    }

    public class MyArray
    {
        public int[] myArray { get; set; }
    }
#pragma warning restore IDE1006 // Naming Styles
}
