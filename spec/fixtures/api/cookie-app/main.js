const { app, session } = require('electron')

app.on('ready', async function () {
  const url = 'http://foo.bar'
  const persistentSession = session.fromPartition('persist:ence-test')
  const name = 'test'
  const value = 'true'

  const set = () => persistentSession.cookies.set({
    url,
    name,
    value,
    expirationDate: Date.now() + 60000
  })

  const get = () => persistentSession.cookies.get({
    url
  })

  const maybeRemove = async (pred) => new Promise(async (resolve, reject) => {
    try {
      if (pred()) {
        await persistentSession.cookies.remove(url, name)
      }
      resolve()
    } catch (error) {
      reject(error)
    }
  })

  try {
    await maybeRemove(() => process.env.PHASE === 'one')
    const one = await get()
    await set()
    const two = await get()
    await maybeRemove(() => process.env.PHASE === 'two')
    const three = await get()

    process.stdout.write(`${one.length}${two.length}${three.length}`)
  } catch (e) {
    process.stdout.write('ERROR')
  } finally {
    process.stdout.end()

    app.quit()
  }
})
