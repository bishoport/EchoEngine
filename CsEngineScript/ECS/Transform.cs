using CsEngineScript.ECS.Attributes;
using System;
using System.Collections.Generic;
using System.Numerics; // Para usar Vector3 y Quaternion, agrega el paquete System.Numerics.Vectors a tu proyecto.


namespace CsEngineScript.ECS
{

    public class Transform : Component
    {
        // Aquí podrías tener un diccionario para almacenar propiedades serializables
        //public Dictionary<string, object> serializableProperties = new Dictionary<string, object>();

        [SerializableProperty]
        public Vector3 Position { get; set; } = Vector3.Zero;
        
        [SerializableProperty]
        public Quaternion Rotation { get; set; } = Quaternion.Identity;
        
        [SerializableProperty]
        public Vector3 Scale { get; set; } = Vector3.One;

        public Vector3 Euler
        {
            get
            {
                // Conversion from quaternion to Euler angles (pitch, yaw, roll)
                var q = Rotation;

                // Roll (X-axis rotation)
                double sinr_cosp = 2 * (q.W * q.X + q.Y * q.Z);
                double cosr_cosp = 1 - 2 * (q.X * q.X + q.Y * q.Y);
                double roll = Math.Atan2(sinr_cosp, cosr_cosp);

                // Pitch (Y-axis rotation)
                double sinp = 2 * (q.W * q.Y - q.Z * q.X);
                double pitch;
                if (Math.Abs(sinp) >= 1)
                    pitch = Math.PI / 2 * Math.Sign(sinp); // Se utiliza Math.Sign para determinar el signo.
                else
                    pitch = Math.Asin(sinp);

                // Yaw (Z-axis rotation)
                double siny_cosp = 2 * (q.W * q.Z + q.X * q.Y);
                double cosy_cosp = 1 - 2 * (q.Y * q.Y + q.Z * q.Z);
                double yaw = Math.Atan2(siny_cosp, cosy_cosp);

                return new Vector3((float)roll, (float)pitch, (float)yaw);
            }
        }


        public Transform()
        {
            Console.WriteLine("PASAMOS POR EL CONSTRUCTOR EN CsTestComponent");
            //SetSerializableProperty("Position", Position);
        }

        public override void Init()
        {
            
        }

        public virtual Dictionary<string, object> Serialize()
        {
            var serializableProperties = new Dictionary<string, object>();

            foreach (var property in this.GetType().GetProperties())
            {
                if (Attribute.IsDefined(property, typeof(SerializablePropertyAttribute)))
                {
                    serializableProperties[property.Name] = property.GetValue(this);
                }
            }

            return serializableProperties;
        }

        public virtual void Deserialize(Dictionary<string, object> data)
        {
            foreach (var property in this.GetType().GetProperties())
            {
                if (Attribute.IsDefined(property, typeof(SerializablePropertyAttribute)))
                {
                    if (data.TryGetValue(property.Name, out var value))
                    {
                        property.SetValue(this, value);
                    }
                }
            }
        }

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

        public override void Update(float deltaTime)
        {
            // Lógica de actualización si es necesaria
        }

        public Matrix4x4 LocalModelMatrix
        {
            get
            {
                return Matrix4x4.CreateScale(Scale) *
                       Matrix4x4.CreateFromQuaternion(Rotation) *
                       Matrix4x4.CreateTranslation(Position);
            }
        }

        public void SetTransform(Matrix4x4 transformMatrix)
        {
            Position = transformMatrix.Translation;
            Rotation = Quaternion.CreateFromRotationMatrix(transformMatrix);
            Scale = new Vector3(transformMatrix.M11, transformMatrix.M22, transformMatrix.M33); // Asumiendo que la matriz es de una transformación pura sin sesgo ni perspectiva
        }

        public override void DrawGUIInspector()
        {
            // Lógica para dibujar el inspector de la GUI, adaptada a las herramientas de GUI de C#
        }
    }
}


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

