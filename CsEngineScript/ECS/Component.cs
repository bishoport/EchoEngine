using System;
using System.Collections.Generic;
using CsEngineScript.ECS.Attributes;

namespace CsEngineScript.ECS
{
    public abstract class Component
    {
        public virtual void Init() { }
        public virtual void Update(float deltaTime) { }
        public virtual void Draw() { }
        public virtual void DrawGUIInspector() { }
        public virtual void OnDestroy() { }
    }
}

