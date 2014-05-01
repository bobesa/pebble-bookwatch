Description
===========

A simple watchface with *page roll animation* & *color invert* on every minute

To test page flipping by select button, you need to set

```json
{
    "watchapp": {
        "watchface": false
    }
}
```
in `appinfo.json` file

Build & Deploy
==============

```bash
 $ pebble build
 $ pebble install --phone PHONE_IP
```

Credits
=======

Programming: __Vlastimil Šenfeld__ ([bobesa](https://github.com/bobesa))

Font: __Brankovic__ by __Amy VanTorre__