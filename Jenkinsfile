// https://github.com/Rudd-O/shared-jenkins-libraries
@Library('shared-jenkins-libraries@master') _

def RELEASE = funcs.loadParameter('parameters.groovy', 'RELEASE', '28')


def srpm_step() {
    return {
        dir('src') {
            script {
                env.GIT_HASH = sh (
                    script: "git rev-parse --short HEAD",
                    returnStdout: true
                ).trim()
                println "Git hash is reported as ${env.GIT_HASH}"
                sh """
                ./autogen.sh
                sed "s/_META_RELEASE=.*/_META_RELEASE=0.${env.BUILD_NUMBER}.${env.GIT_HASH}/" -i configure
                ./configure --with-config=user
                make srpm-dkms srpm-utils
                """
            }
        }
    }
}

genericFedoraRPMPipeline(
    null,
    srpm_step(),
    ['autoconf', 'automake', 'libtool', 'zlib-devel']
)
