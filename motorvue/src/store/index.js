import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)


const store = new Vuex.Store({
  state: {
    slider: 0,
    angle: 0,
    onoff: 1,
    mqtturl: "maqiatto.com",
  }
  // mutations: {

  // },
  // actions: {
  // },
  // modules: {
  // }
})

export default store