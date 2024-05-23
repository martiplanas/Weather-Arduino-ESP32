# Weather-Arduino-ESP32

## To-do
- [ ] Optimize DisplayNumber() if's
- [X] Multi page rewrite
- [ ] Add page 1
- [X] Add page 2
- [X] Add page 3

### 28 Maig Entrega Projecte
    - Enseñar al resta com funciona el programa
    - Video funcionament (no cal tutorial)
    - Codi pujar .ino + copy paste document
    - Explicar com ho hem fet

## Condition map
| Name          | ConditionVar | openWeather icon |
|---------------|--------------|------------------|
| Sun           | 1            | 01d              |
| Sun & Cloud   | 2            | 02d              |
| Cloudy        | 3            | 03d, 04d         |
| Rain          | 4            | 09d, 10d         |
| Thunderstorm  | 5            | 11d              |
| Unknown       | 0            | else             |