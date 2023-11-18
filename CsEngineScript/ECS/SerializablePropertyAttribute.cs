// SerializablePropertyAttribute.cs
using System;

namespace CsEngineScript.ECS.Attributes
{
    [AttributeUsage(AttributeTargets.Property, AllowMultiple = false)]
    public class SerializablePropertyAttribute : Attribute
    {
    }
}

