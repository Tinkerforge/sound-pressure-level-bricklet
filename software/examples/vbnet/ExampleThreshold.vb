Imports System
Imports Tinkerforge

Module ExampleThreshold
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Sound Pressure Level Bricklet

    ' Callback subroutine for decibel callback
    Sub DecibelCB(ByVal sender As BrickletSoundPressureLevel, ByVal decibel As Integer)
        Console.WriteLine("Decibel: " + (decibel/10.0).ToString() + " dB(A)")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim spl As New BrickletSoundPressureLevel(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Register decibel callback to subroutine DecibelCB
        AddHandler spl.DecibelCallback, AddressOf DecibelCB

        ' Configure threshold for decibel "greater than 60 dB(A)"
        ' with a debounce period of 1s (1000ms)
        spl.SetDecibelCallbackConfiguration(1000, False, ">"C, 60*10, 0)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
