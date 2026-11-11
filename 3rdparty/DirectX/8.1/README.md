# DirectX 8.1 SDK

`lib/dinput8.lib` of the DirectX 8.1 SDK (identical in 8.1b): the DirectInput 8 import library with the
`c_dfDIKeyboard` / `c_dfDIMouse` data-format objects. swkotor2.exe takes them from this library, not from the
prerelease `dinput8.lib` of the Visual Studio .NET 2002 Platform SDK that swkotor.exe used: the LINK tool records
of the KOTOR II executable list the data-format objects as Utc13_C 9178 (this library; the prerelease one is 9177)
and the DINPUT8 descriptor objects among the 9210 import objects of the Platform SDK libraries.
