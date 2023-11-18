using System;
using System.Collections;
using System.Collections.Generic;

namespace CsEngineScript.ECS
{
    public class Entity
    {
        private static int maxComponents = 32; // Asegúrate de establecer esto según tu necesidad
        private List<Component> components = new List<Component>();
        private Component[] componentArray = new Component[maxComponents];
        private BitArray componentBitSet = new BitArray(maxComponents);

        public int Id { get; private set; }
        public string Name { get; set; }
        public bool Active { get; set; } = true;
        public bool MarkedToDelete { get; set; } = false;

        public Entity(int nextID)
        {
            Id = nextID;
        }

        public void Update(float deltaTime)
        {
            foreach (var component in components)
            {
                component.Update(deltaTime);
            }
        }

        public void Draw()
        {
            foreach (var component in components)
            {
                component.Draw();
            }
        }

        public void DrawGUIInspector()
        {
            foreach (var component in components)
            {
                component.DrawGUIInspector();
            }
        }

        public IEnumerable<Component> GetComponents()
        {
            return components;
        }

        public void Destroy()
        {
            Active = false;
        }

        public void RemoveAllComponents()
        {
            foreach (var component in components)
            {
                component.OnDestroy();
            }
            components.Clear();
            componentArray = new Component[maxComponents];
            componentBitSet.SetAll(false);
        }

        public void AddComponent(Component component)
        {
            components.Add(component);
            componentArray[GetComponentTypeID(component.GetType())] = component;
            componentBitSet.Set(GetComponentTypeID(component.GetType()), true);
            component.Init();
        }

        public bool HasComponent<T>() where T : Component
        {
            return componentBitSet.Get(GetComponentTypeID(typeof(T)));
        }

        public T GetComponent<T>() where T : Component
        {
            return (T)componentArray[GetComponentTypeID(typeof(T))];
        }

        public void RemoveComponent<T>() where T : Component
        {
            var toRemove = GetComponent<T>();
            components.Remove(toRemove);
            componentArray[GetComponentTypeID(typeof(T))] = null;
            componentBitSet.Set(GetComponentTypeID(typeof(T)), false);
            toRemove.OnDestroy();
        }

        // Deberías implementar GetNewComponentTypeID y GetComponentTypeID de tal manera que devuelvan un índice único para cada tipo de componente.
        private int GetComponentTypeID(Type componentType)
        {
            // Implementación que asigne un índice único a cada tipo de componente
            return 0;
        }
    }
}

//public Dictionary<string, object> Serialize()
//{
//    var entityData = new Dictionary<string, object>
//    {
//        ["Id"] = Id,
//        ["Name"] = Name,
//        // ... otros campos de Entity ...
//    };

//    // Agregar datos de los componentes
//    var componentsData = new List<object>();
//    foreach (var component in components)
//    {
//        componentsData.Add(component.Serialize());
//    }

//    entityData["Components"] = componentsData;

//    return entityData;
//}

//public void Deserialize(Dictionary<string, object> entityData)
//{
//    // Asumiendo que entityData contiene las claves y valores correctos
//    Id = (int)entityData["Id"];
//    Name = (string)entityData["Name"];
//    // ... otros campos de Entity ...

//    var componentsData = (List<object>)entityData["Components"];
//    foreach (var componentData in componentsData)
//    {
//        // Deserializar cada componente y agregarlo a la entidad
//        // Necesitarás una fábrica o algún mecanismo para crear componentes
//        // basados en los datos deserializados
//    }
//}


