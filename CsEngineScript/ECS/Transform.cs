using CsEngineScript.ECS.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text.Json;

namespace CsEngineScript.ECS
{
    public class PropertyData
    {
        public object Value { get; set; }
        public string Type { get; set; }

        public PropertyData(object value, string type)
        {
            Value = value;
            Type = type;
        }
    }

    public class Transform:Component
    {
        [SerializableProperty]
        public Vector3 Position { get; set; } = Vector3.Zero;

        [SerializableProperty]
        public Quaternion Rotation { get; set; } = Quaternion.Identity;

        [SerializableProperty]
        public Vector3 Scale { get; set; } = Vector3.One;

        [SerializableProperty]
        public string Saludo { get; set; } = "HOLA FAMILIA";

        [SerializableProperty]
        public float SomeFloat { get; set; } = 1.0f;

        [SerializableProperty]
        public int SomeInteger { get; set; } = 42;

        [SerializableProperty]
        public bool SomeBoolean { get; set; } = true;

        public Transform()
        {
            Console.WriteLine("PASAMOS POR EL CONSTRUCTOR EN CS Transform");
            Position = new Vector3(3.0f, 0.0f, 0.0f);
        }

        public string SerializeToJson()
        {
            var options = new JsonSerializerOptions();
            options.Converters.Add(new Vector3JsonConverter());
            options.Converters.Add(new QuaternionJsonConverter());
            // Añade convertidores adicionales para otros tipos si es necesario.

            var serializableProperties = GetSerializableProperties();
            string json = JsonSerializer.Serialize(serializableProperties, options);
            return json;
        }

        private Dictionary<string, PropertyData> GetSerializableProperties()
        {
            var properties = new Dictionary<string, PropertyData>();

            foreach (var property in GetType().GetProperties())
            {
                if (Attribute.IsDefined(property, typeof(SerializablePropertyAttribute)))
                {
                    properties[property.Name] = new PropertyData(
                        property.GetValue(this),
                        property.PropertyType.Name
                    );
                }
            }
            return properties;
        }



        public override void Init()
        {
            base.Init();
            Console.WriteLine("PASAMOS POR EL INIT EN CS Transform");
        }

        public override void Update(float deltaTime)
        {
            base.Update(deltaTime);
        }

        public override void Draw()
        {
            base.Draw();
        }

        public override void DrawGUIInspector()
        {
            base.DrawGUIInspector();
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
        }
    }
}













//public Dictionary<string, object> GetSerializableProperties()
//{
//    return serializableProperties;
//}

//public KeyValuePair<string, object>[] GetSerializablePropertiesArray()
//{
//    return serializableProperties.ToArray();
//}



//public virtual Dictionary<string, object> Serialize()
//{
//    // Crear una nueva instancia del diccionario para serializar
//    var serializedProperties = new Dictionary<string, object>();

//    foreach (var kvp in serializableProperties)
//    {
//        serializedProperties[kvp.Key] = kvp.Value;
//    }

//    return serializedProperties;
//}

//public virtual void Deserialize(Dictionary<string, object> data)
//{
//    // Crear una copia del diccionario para la deserialización
//    var dataCopy = new Dictionary<string, object>(data);
//    foreach (var kvp in dataCopy)
//    {
//        if (serializableProperties.ContainsKey(kvp.Key))
//        {
//            var property = GetType().GetProperty(kvp.Key);
//            if (property != null && Attribute.IsDefined(property, typeof(SerializablePropertyAttribute)))
//            {
//                property.SetValue(this, kvp.Value);
//            }
//        }
//    }
//}



//public  void Update(float deltaTime)
//{
//    // Lógica de actualización si es necesaria
//}

//public Matrix4x4 LocalModelMatrix
//{
//    get
//    {
//        return Matrix4x4.CreateScale(Scale) *
//               Matrix4x4.CreateFromQuaternion(Rotation) *
//               Matrix4x4.CreateTranslation(Position);
//    }
//}

//public void SetTransform(Matrix4x4 transformMatrix)
//{
//    Position = transformMatrix.Translation;
//    Rotation = Quaternion.CreateFromRotationMatrix(transformMatrix);
//    Scale = new Vector3(transformMatrix.M11, transformMatrix.M22, transformMatrix.M33); // Asumiendo que la matriz es de una transformación pura sin sesgo ni perspectiva
//}

//public  void DrawGUIInspector()
//{

//}


// Método para obtener una propiedad serializable
//public object GetSerializableProperty(string propertyName)
//{
//    if (serializableProperties.ContainsKey(propertyName))
//    {
//        return serializableProperties[propertyName];
//    }
//    else
//    {
//        Console.WriteLine("Property not found: " + propertyName);
//    }
//    return null;
//}

//// Método para establecer una propiedad serializable
//public void SetSerializableProperty(string propertyName, object value)
//{
//    serializableProperties[propertyName] = value;
//}




//public override Dictionary<string, object> Serialize()
//{
//    return new Dictionary<string, object>
//    {
//        {"Position", Position},
//        {"Rotation", Rotation},
//        {"Scale", Scale}
//    };
//}

//public override void Deserialize(Dictionary<string, object> componentData)
//{
//    // Aquí tendrías que convertir los valores del diccionario de nuevo a los tipos adecuados.
//    //Position = ConvertToVector3(componentData["Position"]);
//    //Rotation = ConvertToQuaternion(componentData["Rotation"]);
//    //Scale = ConvertToVector3(componentData["Scale"]);
//}

