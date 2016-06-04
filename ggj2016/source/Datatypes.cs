using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace jam.source
{
    class Ref <T>
    {
        public T Value { get; set; }

        public static implicit operator T(Ref<T> reff)
        {
            return reff.Value;
        }
        public static implicit operator Ref<T>(T t)
        {
            var r = new Ref<T>();
            r.Value = t;
            return r;
        }

    }
}
