Imports System
Imports Tinkerforge

Module ExampleSimple
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Sound Pressure Level Bricklet

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim spl As New BrickletSoundPressureLevel(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Get current decibel
        Dim decibel As Integer = spl.GetDecibel()
        Console.WriteLine("Decibel: " + (decibel/10.0).ToString() + " dB(A)")

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
