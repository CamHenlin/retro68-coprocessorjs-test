const puppeteer = require('puppeteer')

class PuppeteerTest {

  async getBodyAtURL (url) {

    let response

    try {

      const browser = await puppeteer.launch()
      const page = await browser.newPage()

      await page.goto(url)

      response = await page.$eval('*', el => el.innerText)

      await browser.close()
    } catch (error) {

      console.error(error)
    }

    return response
  }
}
module.exports = PuppeteerTest