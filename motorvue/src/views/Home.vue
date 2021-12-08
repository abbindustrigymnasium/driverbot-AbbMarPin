<template>
  <v-container fluid>
    <!-- gamepad setup -->
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
              <v-dialog v-model="dialog" width="500">
                <template v-slot:activator="{ on }">
                  <v-btn dark v-on="on">Mqtt config</v-btn>
                </template>

                <v-card>
                  <v-card-title class="headline" primary-title>MQTT CONFIG</v-card-title>
                  <v-divider></v-divider>

                  <v-form v-model="valid">
                    <v-container>
                      <v-row>
                        <v-col cols="12" md="4">
                          <v-text-field
                            v-model="temp_creds.url"
                            :rules="urlrules"
                            label="URL"
                            required
                          ></v-text-field>
                        </v-col>

                        <v-col cols="12" md="4">
                          <v-text-field
                            v-model="temp_creds.port"
                            :rules="portrules"
                            type="number"
                            label="Port"
                            required
                          ></v-text-field>
                        </v-col>

                        <v-col cols="12" md="4">
                          <v-text-field
                            v-model="temp_creds.username"
                            :rules="userrules"
                            label="MQTT username"
                            required
                          ></v-text-field>
                        </v-col>

                        <v-col cols="12" md="4">
                          <v-text-field
                            v-model="temp_creds.password"
                            label="MQTT password"
                            type="password"
                            required
                          ></v-text-field>
                        </v-col>

                        <v-col cols="12" md="4">
                          <v-text-field
                            v-model="temp_creds.motor_topic"
                            :rules="userrules"
                            label="MQTT Motor topic"
                            required
                          ></v-text-field>
                        </v-col>

                        <v-col cols="12" md="4">
                          <v-text-field
                            v-model="temp_creds.servo_topic"
                            :rules="userrules"
                            label="MQTT Servo topic"
                            required
                          ></v-text-field>
                        </v-col>
                      </v-row>
                    </v-container>
                  </v-form>

                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn
                      color="primary"
                      text
                      :disabled="!valid"
                      @click="mqtt_state = false; mqttcontrol(); dialog = false"
                    >save</v-btn>
                  </v-card-actions>
                </v-card>
              </v-dialog>
              <v-switch
                label="Connected to MQTT"
                :color="mqtt_state ? 'success' : 'error'"
                v-model="mqtt_state"
                @change="mqttcontrol()"
                :loading="(mqtt_state && !connected)"
              ></v-switch>
            </v-col>
          </v-row>
        </v-card-title>
        <v-container v-if="connected" grid-list-xs>
          <v-radio-group v-model="controloption">
            <v-radio v-for="n in controloptions" :key="n" :label="`${n}`" :value="n"></v-radio>
          </v-radio-group>
          <v-switch v-model="onoff" class="ma-2" label="Armed"></v-switch>
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
          <v-card-actions v-else-if="controloption == controloptions[1]">
            <span>Please connect an gamepad that's compadible with your browser</span>
          </v-card-actions>
        </v-container>
        <v-alert type="error" class="mx-4" :value="!connected">Not connected!</v-alert>
      </v-card>
    </v-row>
  </v-container>
</template>

<script>
// @ is an alias to /src
// import HelloWorld from "@/components/HelloWorld.vue";
var mqtt = require("mqtt");

// import {mapGetters} from 'vuex'
// import {mapActions} from 'vuex'

// kopplar upp sig, startar loop
// tar in gamepad/slidervärden, packar in dem och skickar
// kör igen
// kopplar bort sig och stoppar loopen


export default {
  name: "Home",
  components: {
    // HelloWorld
  },
  data: () => ({
    temp_creds: {
      url: "",
      port: undefined,
      username: "",
      password: "",
      motor_topic: "",
      servo_topic: ""
    },
    creds: {
      url: "maqiatto.com",
      port: 8883,
      username: "martin.pind@abbindustrigymnasium.se",
      password: "loaldoaldoawldoakdfigvjosgoshnbos",
      motor_topic: "martin.pind@abbindustrigymnasium.se/motor",
      servo_topic: "martin.pind@abbindustrigymnasium.se/servo"
    },
    slider: 0,
    angle: 0,
    onoff: 1,
    connected: false,
    client: undefined,
    controloption: "sliders",
    controloptions: ["sliders", "gamepad"],
    dialog: false,
    turnleft: false,
    turnright: false,
    forward: false,
    reverse: false,
    urlrules: [
      v => !!v || "URL is required",
      v => /.+\..+/.test(v) || "URL must be valid"
    ],
    portrules: [v => !!v || "Port is required"],
    userrules: [
      v => !!v || "E-mail is required",
      v => /.+@.+\..+/.test(v) || "E-mail must be valid"
    ],
    valid: false,
    log: "",
    mqtt_state: false,
    interval: undefined,
    snacc: false
  }),
  computed: {
    // ...mapGetters(["slider", "angle", "onoff", "mqtturl"])
  },
  mounted() {},
  methods: {
    send() {
      this.client.publish(
        // "martin.pind@abbindustrigymnasium.se/motor",
        this.creds.motor_topic,
        this.composemotor() // skicka det formaterade värdena
      );
      this.client.publish(
        // "martin.pind@abbindustrigymnasium.se/servo",
        this.creds.servo_topic,
        this.composeservo()
      );
    },
    mqttcontrol() { // kopplar upp och ned
      if (this.mqtt_state) {
        this.connect();
      } else {
        clearInterval(this.interval);
        if (this.connected) {
          this.client.end();
        }
      }
    },
    composemotor() { // tar in motorvärden och formaterar ett förflyttningspaket
      let Direction = 0;
      let speed = 0;
      if (this.controloption == this.controloptions[0]) {
        this.onoff = this.onoff ? 1 : 0;

        if (this.slider > 0) {
          Direction = 1;
          speed = Math.round(this.slider);
        } else if (this.slider < 0) {
          Direction = 0;
          speed = Math.round(this.slider) * -1;
        }
        return `(${this.onoff},${Direction},${speed})`;
      } else if (this.controloption == this.controloptions[1]) {
        if (this.forward) {
          Direction = this.forward ? 1 : 0;
          speed = this.forward ? 100 : 0;
        } else {
          Direction = this.reverse ? 0 : 1;
          speed = this.reverse ? 100 : 0;
        }
        // console.log(`(${this.onoff},${Direction},${speed})`)
        return `(${this.onoff},${Direction},${speed})`;
      }
    },
    composeservo() { // tar in servo värden och formaterar ett styrpaket
      let angle = 90;
      if (this.controloption == this.controloptions[0]) {
        angle = Math.round(this.map_range(this.angle, -100, 100, 180, 0));
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
    connect() { // kopplar upp sig
      let self = this; 
      var mqtt_url = this.creds.url;
      var url = "mqtt://" + mqtt_url;
      var options = {
        port: this.creds.port, // mqtt saker
        clientId:
          "mqttjs_" +
          Math.random()
            .toString(16)
            .substr(2, 8),
        // username: "martin.pind@abbindustrigymnasium.se",
        // password: "loaldoaldoawldoakdfigvjosgoshnbos"
        username: this.creds.username,
        password: this.creds.password
      };
      console.log("connecting");
      this.client = mqtt.connect(url, options);
      this.client
        .on("connect", function() {
          console.log("connected!!!");
          // self.callback_(true)
          self.interval = setInterval(() => { // starta skicka loopen
            if (self.mqtt_state) {
              self.send(); // skicka om den ska det
            }
            self.turnleft = false;
            self.turnright = false; // nollställ kontroller värden
            self.forward = false;
            self.reverse = false;
          }, 200);
          self.connected = true;
        })
        .on("message", function(topic, message) {
          // message is Buffer
          console.log(message.toString());
        })
        .on("error", function(error) {
          console.log(error);
          self.connected = false;
        })
        .on("close", function(error) {
          // console.log(error);
          console.log("closed");
          self.connected = false;
        });
    },
    reset() {
      this.slider = 0;
      this.onoff = 0; // reset knapp
      this.angle = 0;
    },
    map_range(value, low1, high1, low2, high2) { // maprange som i arduino
      return low2 + ((high2 - low2) * (value - low1)) / (high1 - low1);
    },
    callback_(state) { // test funktion
      // console.log("AAAAAAAAAAAA");
      this.connected = state;
    },
    saveconf() { // spara creds
      this.creds = this.temp_creds;
    },
    _turnleft() { // kontroller funktioner
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
