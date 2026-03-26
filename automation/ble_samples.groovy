def verify_checkpatch(){
     sh '''#!/bin/bash -xe
        env
        cd /root/alif
        west forall -c "git clean -fdx"
        cd alif
        git checkout main
        git fetch -pu
        git reset --hard origin/main
        west update
        cd /root/alif/modules/hal/alif
        if [[ -v CHANGE_ID ]]; then
            git fetch -pu alif
            git branch -D pr-${CHANGE_ID} || true
            git fetch alif pull/${CHANGE_ID}/head:pr-${CHANGE_ID}
            git checkout pr-${CHANGE_ID}
            /root/alif/zephyr/scripts/checkpatch.pl --ignore=GERRIT_CHANGE_ID,EMAIL_SUBJECT,COMMIT_MESSAGE,COMMIT_LOG_LONG_LINE -g pr-\${CHANGE_ID}...alif/main
            STATUS=\$?
            if [ \$STATUS -ne 0 ]; then
                exit \$STATUS
            else
                echo "Checkpatch passed successfully"
            fi
        else
            git fetch alif -pu
            git reset --hard alif/main
        fi
        cd ..
        '''
}

def verify_gitlint (){
    sh '''#!/bin/bash -xe
        env
        cd /root/alif
        west forall -c "git clean -fdx"
        cd alif
        git checkout main
        git fetch -pu
        git reset --hard origin/main
        west update
        cd /root/alif/modules/hal/alif
        if [[ -v CHANGE_ID ]]; then
            git fetch -pu alif
            git branch -D pr-${CHANGE_ID} || true
            git fetch alif pull/${CHANGE_ID}/head:pr-${CHANGE_ID}
            git checkout pr-${CHANGE_ID}
        else
            git checkout alif/main
            git reset --hard alif/main
            git pull
        fi
        cd ..
        cd /root/alif/modules/hal/alif
        pip install gitlint
        git log -$(git rev-list --count alif/main..HEAD) --pretty=%B | gitlint
        exit $?
        '''
}

return [

    verify_checkpatch: this.&verify_checkpatch,
    verify_gitlint: this.&verify_gitlint,
]
