using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CsEngineScript.ECS
{
    public class CsTestComponent : Component
    {
        // Aquí podrías tener un diccionario para almacenar propiedades serializables
        public  Dictionary<string, object> serializableProperties = new Dictionary<string, object>();

        public string pruebaPropiedad = "Hola soy una propiedad en CsTestComponent";

        public CsTestComponent()
        {
            Console.WriteLine("PASAMOS POR EL CONSTRUCTOR EN CsTestComponent");
            SetSerializableProperty("pruebaPropiedad", pruebaPropiedad);
        }

        public override void Init()
        {
            Console.WriteLine("Init en CsTestComponent");
            base.Init();
        }

        public override void Update(float deltaTime)
        {
            // Ejemplo de actualización de la propiedad en el tiempo
            pruebaPropiedad = "Valor actualizado en Update";
            SetSerializableProperty("pruebaPropiedad", pruebaPropiedad);
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

        // Método para obtener una propiedad serializable
        public object GetSerializableProperty(string propertyName)
        {
            Console.WriteLine("GetSerializableProperty: " + propertyName);
            if (serializableProperties.ContainsKey(propertyName))
            {
                return serializableProperties[propertyName];
            }
            else
            {
                Console.WriteLine("Property not found: " + propertyName);
            }
            return null;
        }

        // Método para establecer una propiedad serializable
        public void SetSerializableProperty(string propertyName, object value)
        {
            serializableProperties[propertyName] = value;
        }









        public override bool Equals(object obj)
        {
            return base.Equals(obj);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        

        public override void OnDestroy()
        {
            base.OnDestroy();
        }

        public override string ToString()
        {
            return base.ToString();
        }

        //public override Dictionary<string, object> Serialize()
        //{
        //    //...throw new NotImplementedException();
        //}

        //public override void Deserialize(Dictionary<string, object> componentData)
        //{
        //    throw new NotImplementedException();
        //}
    }
}
