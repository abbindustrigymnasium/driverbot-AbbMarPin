<template>
  <v-container fluid>
    <a v-gamepad:left-analog-left.repeat="_turnleft" />
    <a v-gamepad:left-analog-right.repeat="_turnright" />
    <a v-gamepad:trigger-right.repeat="_forward" />
    <a v-gamepad:trigger-left.repeat="_reverse" />

    <v-row justify="space-around">
      <v-card>
        <v-card-title primary-title>
          <v-row>
            <v-col cols="12">
              <h3 class="headline mb-0">Motor</h3>
            </v-col>
            <v-col cols="12">
              <v-radio-group v-model="controloption">
                <v-radio v-for="n in controloptions" :key="n" :label="`${n}`" :value="n"></v-radio>
              </v-radio-group>
              <v-switch v-model="onoff" class="ma-2" label="Armed"></v-switch>
            </v-col>
          </v-row>
        </v-card-title>
        <v-card-actions v-if="controloption == controloptions[0]">
          <v-row>
            <v-col cols="12">
              <v-btn @click="reset()">reset</v-btn>
            </v-col>
            <v-col cols="12">
              <v-slider v-model="slider" min="-100" max="100" label="hastighet"></v-slider>
            </v-col>
            <v-col cols="12">
              <v-slider v-model="angle" min="-100" max="100" label="styr"></v-slider>
            </v-col>
          </v-row>
        </v-card-actions>
        <v-card-actions v-if="controloption == controloptions[1]">
          <span>Please connect an gamepad that's compadible with this browser</span>
        </v-card-actions>
        <v-alert type="error" :value="!connected">Not connected!</v-alert>
      </v-card>
    </v-row>
  </v-container>
</template>

<script>
// @ is an alias to /src
// import HelloWorld from "@/components/HelloWorld.vue";
var mqtt = require("mqtt");

export default {
  name: "Home",
  components: {
    // HelloWorld
  },
  data: () => ({
    slider: 0,
    angle: 0,
    onoff: 1,
    mqtturl: "maqiatto.com",
    connected: false,
    client: undefined,
    controloption: "gamepad",
    controloptions: ["sliders", "gamepad"],
    turnleft: false,
    turnright: false,
    forward: false,
    reverse: false
  }),
  created() {
    this.connect();
  },
  mounted() {
    setInterval(() => {
      this.send();
      this.turnleft = false;
      this.turnright = false;
      this.forward = false;
      this.reverse = false;
    }, 200);
  },
  methods: {
    send() {
      this.client.publish(
        "martin.pind@abbindustrigymnasium.se/motor",
        this.composemotor()
      );

      this.client.publish(
        "martin.pind@abbindustrigymnasium.se/servo",
        this.composeservo()
      );
    },
    composemotor() {
      let Direction = 0;
      let speed = 0;
      if (this.controloption == this.controloptions[0]) {
        if (this.onoff) {
          this.onoff = 1;
        } else {
          this.onoff = 0;
        }

        if (this.slider > 0) {
          Direction = 0;
          speed = this.slider;
        } else if (this.slider < 0) {
          Direction = 1;
          speed = this.slider * -1;
        }
        return `(${this.onoff},${Direction},${speed})`;
      } else if (this.controloption == this.controloptions[1]) {
        if (this.forward) {
          Direction = this.forward ? 0 : 1;
          speed = this.forward ? 100 : 0;
        } else {
          Direction = this.reverse ? 1 : 0;
          speed = this.reverse ? 100 : 0;
        }
        return `(${this.onoff},${Direction},${speed})`;
      }
    },
    composeservo() {
      let angle = 90;
      if (this.controloption == this.controloptions[0]) {
        let angle = this.map_range(this.angle, -100, 100, 180, 0);
      } else if (this.controloption == this.controloptions[1]) {
        if (this.turnleft) {
          angle = this.turnleft ? -100 : 0;
        } else {
          angle = this.turnright ? 100 : 0;
        }

        angle = this.map_range(angle, -100, 100, 180, 0);
      }
      return `${angle}`;
    },
    connect() {
      var mqtt_url = this.mqtturl;
      var url = "mqtt://" + mqtt_url;
      var options = {
        port: 8883,
        clientId:
          "mqttjs_" +
          Math.random()
            .toString(16)
            .substr(2, 8),
        username: "martin.pind@abbindustrigymnasium.se",
        password: "loaldoaldoawldoakdfigvjosgoshnbos"
      };
      console.log("connecting");
      this.client = mqtt.connect(url, options);
      this.client
        .on("connect", function() {
          console.log("connected!!!");
          this.connected = true;
        })
        .on("message", function(topic, message) {
          // message is Buffer
          console.log(message.toString());
        })
        .on("error", function(error) {
          console.log(error);
          this.connected = false;
        })
        .on("close", function(error) {
          console.log(error);
          console.log("closed");
          this.connected = false;
        });
      this.connected = true;
    },
    reset() {
      this.slider = 0;
      this.onoff = 0;
      this.angle = 0;
    },
    map_range(value, low1, high1, low2, high2) {
      return low2 + ((high2 - low2) * (value - low1)) / (high1 - low1);
    },
    callback() {
      console.log("AAAAAAAAAA");
    },
    _turnleft() {
      this.turnleft = true;
      // console.log("left")
    },
    _turnright() {
      this.turnright = true;
    },
    _forward() {
      this.forward = true;
    },
    _reverse() {
      this.reverse = true;
    }
  }
};
</script>
