using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Xml;


namespace CsEngineScript.ECS
{
    public class Manager
    {
        private List<Entity> entities = new List<Entity>();
        private int nextID = 0;

        public void Update(float deltaTime)
        {
            foreach (var entity in entities)
            {
                entity.Update(deltaTime);
            }
        }

        public void Draw()
        {
            foreach (var entity in entities)
            {
                entity.Draw();
            }
        }

        public void DrawGUIInspector()
        {
            foreach (var entity in entities)
            {
                entity.DrawGUIInspector();
            }
        }

        public void Refresh()
        {
            entities.RemoveAll(e => e.MarkedToDelete);
        }

        public Entity AddEntity()
        {
            var entity = new Entity(nextID++);
            entities.Add(entity);
            return entity;
        }

        public void RemoveEntity(Entity entity)
        {
            if (entity == null)
            {
                Console.WriteLine("Attempted to remove a null entity.");
                return;
            }

            entity.Destroy(); // Mark the entity as inactive before removing it.
            entities.Remove(entity);
        }

        public IEnumerable<Entity> GetAllEntities()
        {
            return entities.Where(e => e.Active).ToList();
        }

        
    }
}

//using YamlDotNet.Serialization;
//using YamlDotNet.Serialization.NamingConventions;
//public void SerializeEntities(string filename)
//{
//    //var serializer = new SerializerBuilder()
//    //    .WithNamingConvention(UnderscoredNamingConvention.Instance)
//    //    .Build();

//    //var serializedEntities = entities.Where(e => e.Active).Select(e => e.Serialize()).ToList();
//    //var yaml = serializer.Serialize(serializedEntities);

//    //File.WriteAllText(filename, yaml);
//}

//public void DeserializeEntities(string filename)
//{
//    //var deserializer = new DeserializerBuilder()
//    //    .WithNamingConvention(UnderscoredNamingConvention.Instance)
//    //    .Build();

//    //var yaml = File.ReadAllText(filename);
//    //var deserializedEntities = deserializer.Deserialize<List<Dictionary<string, object>>>(yaml);

//    //foreach (var entityData in deserializedEntities)
//    //{
//    //    var entity = AddEntity();
//    //    entity.Deserialize(entityData);
//    //}
//}

