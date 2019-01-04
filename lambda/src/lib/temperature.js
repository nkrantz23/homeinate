class Temperature {
  constructor(celsius) {
    this.celsius = Number(celsius);
  }

  toFahrenheit() {
    return (this.celsius * (9 / 5)) + 32;
  }

  toString() {
    return `${this.celsius.toFixed(0)} C`;
  }

  toStringFahrenheit() {
    return `${this.toFahrenheit().toFixed(0)} F`;
  }
}

module.exports = {
  Temperature
}